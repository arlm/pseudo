// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\Array.hpp>
#include <Pseudo\Math.hpp>

namespace Pseudo
{
    class HashHelper
    {
        private: static Int primes[72]; // See table below

        public: static Int GetPrime(Int min)        
        {
            for (Int i = 0; i < ARRAY_SIZE(primes); i++)
            {
                Int j = primes[i];
                
                if (j >= min)
                {
                      return j;
                }
            }
            
            for (Int i = min | 1; i < 0x7fffffff; i += 2)
            {
                if (IsPrime(i))
                {
                    return i;
                }
            }

            return min;
        }

        public: static bool IsPrime(Int candidate)
        {
            if ((candidate & 1) == 0)
            {
                return (candidate == 2);
            }
            
            Int i = (Int)Math::Sqrt((Double)candidate);
            
            for (Int j = 3; j <= i; j += 2)
            {
                if ((candidate % j) == 0)
                {
                      return false;
                }
            }
            
            return true;
        }
    };


    __declspec(selectany) Int HashHelper::primes[] = 
    {
        3, 7, 11, 0x11, 0x17, 0x1d, 0x25, 0x2f, 
        0x3b, 0x47, 0x59, 0x6b, 0x83, 0xa3, 0xc5, 0xef, 
        0x125, 0x161, 0x1af, 0x209, 0x277, 0x2f9, 0x397, 0x44f, 
        0x52f, 0x63d, 0x78b, 0x91d, 0xaf1, 0xd2b, 0xfd1, 0x12fd, 
        0x16cf, 0x1b65, 0x20e3, 0x2777, 0x2f6f, 0x38ff, 0x446f, 0x521f, 
        0x628d, 0x7655, 0x8e01, 0xaa6b, 0xcc89, 0xf583, 0x126a7, 0x1619b, 
        0x1a857, 0x1fd3b, 0x26315, 0x2dd67, 0x3701b, 0x42023, 0x4f361, 0x5f0ed, 
        0x72125, 0x88e31, 0xa443b, 0xc51eb, 0xec8c1, 0x11bdbf, 0x154a3f, 0x198c4f, 
        0x1ea867, 0x24ca19, 0x2c25c1, 0x34fa1b, 0x3f928f, 0x4c4987, 0x5b8b6f, 0x6dda89
    };
    
    template<class T> class DefaultTypeComparer
    {
        private: DefaultTypeComparer() {}
        
        public: static Bool Equals(T x, T y)
        {
            return x == y;
        }
        
        public: static IntPtr GetHashCode(T x)
        {
            return (IntPtr)x;
        }
    };

    template<class TKey, class TValue, class TComparer = DefaultTypeComparer<TKey> > class Dictionary
    {
        public: class Enumerator
        {
            private: friend class Dictionary;
            private: Dictionary<TKey, TValue, TComparer>& _dict;
            private: IntPtr _index;
            protected: Enumerator(Dictionary<TKey, TValue, TComparer>& parent) : _dict(parent), _index(-1) {}
            
            public: bool MoveNext()
            {
                if(_index >= _dict.count)
                    return false;
                _index++;
                while((_index < _dict.count) &&
                      (_dict.pEntries->get_Ptr()[_index].hashCode < 0))
                {
                    _index++;
                }
                if(_index >= _dict.count)
                    return false;
                
                return true;
            }

            public: TKey get_Key()
            {
                if(_index<0 || _index >= _dict.count)
                    throw Pseudo::ArgumentOutOfRangeException();
                return _dict.pEntries->get_Ptr()[_index].key;
            }

            public: TValue get_Value()
            {
                if(_index<0 || _index >= _dict.count)
                    throw Pseudo::ArgumentOutOfRangeException();
                return _dict.pEntries->get_Ptr()[_index].value;
            }
        };

        
        friend class Enumerator;

        public: struct Entry
        {
            IntPtr hashCode;
            IntPtr next;
            TKey key;
            TValue value;
        };

        public: Dictionary()
        {
            Initialize(0);
        }

        public: Dictionary(Int capacity)
        {
            Initialize(capacity);
        }
        
        public: ~Dictionary()
        {
            delete pEntries;
            delete pBuckets;
        }

        public: void Add(TKey key, TValue value)
        {
            Insert(key, value, true);
        }
        
        public: void Insert(TKey key, TValue value, bool add)
        {
            IntPtr hashCode = Math::Abs(TComparer::GetHashCode(key));
            IntPtr i;
            
            for (i = pBuckets->GetValue(hashCode % pBuckets->get_Count()); i >= 0; i = pEntries->get_Ptr()[i].next)
            {
                if ((pEntries->get_Ptr()[i].hashCode == hashCode) && TComparer::Equals(pEntries->get_Ptr()[i].key, key))
                {
                    if (add)
                    {
                        throw ArgumentException(); // Adding a duplicate
                    }
                    
                    pEntries->get_Ptr()[i].value = value;
                    
                    return;
                }
            }

            if (freeCount > 0)
            {
                i = freeList;
                freeList = pEntries->get_Ptr()[i].next;
                freeCount--;
            }
            else
            {
                if (pEntries->get_Count() == count)
                {
                    Resize();
                }
                
                i = count;
                count++;
            }
            
            int j = static_cast<Int>(hashCode % pBuckets->get_Count());
            
            Entry& entry = pEntries->get_Ptr()[i];
            
            entry.hashCode = hashCode;
            entry.next = pBuckets->GetValue(j);
            entry.key = key;
            entry.value = value;
            
            pBuckets->SetValue(j, i);
        }
        
        private: void Resize()
        {
            int n = HashHelper::GetPrime(count * 2);
            
            Array<IntPtr>* pNewBuckets = PSEUDO_NEW Array<IntPtr>(n);
            
            for (Int i = 0; i < n; i++)
            {
                pNewBuckets->SetValue(i, -1);
            }
    
            Array<Entry>* pNewEntries = PSEUDO_NEW Array<Entry>(n);
            
            pEntries->CopyTo(*pNewEntries, count);
            
            for (Int i = 0; i < count; i++)
            {
                Entry& entry = pNewEntries->get_Ptr()[i];
                Int m = entry.hashCode % n;
                
                entry.next = pNewBuckets->GetValue(m);
                
                pNewBuckets->SetValue(m, i);
            }

            delete pBuckets;
            delete pEntries;

            pBuckets = pNewBuckets;
            pEntries = pNewEntries;
        }
        
        public: Int get_Count()
        {
            return count - freeCount;
        }
        
        public: TValue get_Item(TKey key)
        {
            IntPtr i = this->FindEntry(key);
    
            if (i < 0)
            {
                throw Exception();
            }
            
            return pEntries->get_Ptr()[i].value;
        }
        
        public: Enumerator* GetEnumerator()
        {
            return PSEUDO_NEW Enumerator(*this);
        }

        public: void get_EntryByOrd(Int entry, TKey *pKey, TValue *pValue)
        {
            Entry &entry2 = pEntries->get_Ptr()[entry];
            *pKey = entry2.key;
            *pValue = entry2.value;
        }
        
        public: void set_Item(TKey key, TValue value)
        {
            Insert(key, value, false);
        }
        
        public: Bool TryGetValue(TKey key, TValue& value)
        {
            IntPtr i = FindEntry(key);
            
            if (i < 0)
            {
                return false;
            }

            value = pEntries->get_Ptr()[i].value;
            return true;
        }
        
        public: Bool ContainsKey(TKey key)
        {
            return (FindEntry(key) >= 0);
        }
        
        public: void Clear()
        {
            if (count > 0)
            {
                for (int i = 0; i < pBuckets->get_Count(); i++)
                {
                    pBuckets->GetValue(i) = -1;
                }
                
                pEntries->set_Count(0);
                freeList = -1;
                count = 0;
                freeCount = 0;
            }
        }

        public: Bool Remove(TKey key)
        {
            if (pBuckets != null)
            {
                IntPtr hashCode = Math::Abs(TComparer::GetHashCode(key));
                Int j = static_cast<int>(hashCode % pBuckets->get_Count());
                IntPtr k = -1;

                for (IntPtr i = pBuckets->GetValue(j); i >= 0; i = pEntries->get_Ptr()[i].next)
                {
                    if ((pEntries->get_Ptr()[i].hashCode == hashCode) && TComparer::Equals(pEntries->get_Ptr()[i].key, key))
                    {
                        if (k < 0)
                        {
                            pBuckets->GetValue(j) = pEntries->get_Ptr()[i].next;
                        }
                        else
                        {
                            pEntries->get_Ptr()[k].next = pEntries->get_Ptr()[i].next;
                        }
                        
                        pEntries->get_Ptr()[i].hashCode = -1;
                        pEntries->get_Ptr()[i].next = freeList;
                        pEntries->get_Ptr()[i].key = TKey();
                        pEntries->get_Ptr()[i].value = TValue();
                        freeList = i;
                        freeCount++;
                        
                        return true;
                    }
                    
                    k = i;
                }
            }

            return false;
        }

        private: IntPtr FindEntry(TKey key)
        {
            IntPtr hashCode = Math::Abs(TComparer::GetHashCode(key));
            
            for (IntPtr i = pBuckets->get_Ptr()[hashCode % pBuckets->get_Count()]; i >= 0; i = pEntries->get_Ptr()[i].next)
            {
                if ((pEntries->get_Ptr()[i].hashCode == hashCode) && TComparer::Equals(pEntries->get_Ptr()[i].key, key))
                {
                    return i;
                }
            }
            
            return -1;
        }
        
        private: void Initialize(Int capacity)
        {
            int prime = HashHelper::GetPrime(capacity);

            pBuckets = PSEUDO_NEW Array<IntPtr>(static_cast<Int>(prime));            
            
            for (int j = 0; j < prime; j++)
            {
                pBuckets->GetValue(j) = -1;
            }
            
            pEntries = PSEUDO_NEW Array<Entry>(prime);
            
            freeList = -1;
            count = 0;
            freeCount = 0;
        }
        
        private: Array<Entry>* pEntries;
        private: IntPtr freeList;
        private: Array<IntPtr>* pBuckets;
        private: Int count;
        private: Int freeCount;
    };
}

#pragma warning(pop)
