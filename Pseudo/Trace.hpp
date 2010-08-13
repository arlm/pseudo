// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\ArrayList.hpp>
#include <Pseudo\TraceListener.hpp>
#include <Pseudo\CountedPtr.hpp>

namespace Pseudo
{
    class TraceListenersCollection
    {
        friend class Trace;
    
        private: ArrayList<CountedPtr<TraceListener> > listeners;
        
        public: void Add(TraceListener* pListener)
        {
            listeners.Add(CountedPtr<TraceListener>(pListener));
        }
        
        public: Int get_Count() const
        {
            return listeners.get_Count();
        }
        
        private: CountedPtr<TraceListener> operator[](Int i) const
        {
            return this->listeners[i];
        }
    };

	class Trace
	{
		private: Trace()
		{
		}

 		public: static void Write(const Char *pMessage)
		{
		    for (int i = 0; i < traceListeners.get_Count(); i++)
		    {
		        CountedPtr<TraceListener> listener(traceListeners[i]);
		        
		        listener.get_Ptr()->Write(pMessage);
		    }
		}

		public: static void WriteLine(const Char* pMessage)
		{
		    for (int i = 0; i < traceListeners.get_Count(); i++)
		    {
		        CountedPtr<TraceListener> listener(traceListeners[i]);
		        
		        listener.get_Ptr()->WriteLine(pMessage);
		    }
		}

		public: static TraceListenersCollection& get_Listeners()
		{
		    return traceListeners;
		}
		
		private: static TraceListenersCollection traceListeners;
	};
	
	__declspec(selectany) TraceListenersCollection Trace::traceListeners;
}

#pragma warning(pop)
