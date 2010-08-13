// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)
#include <Pseudo\Compiler.hpp>
#include <Pseudo\RegistryKey.hpp>

namespace Pseudo
{
    class Registry
    {
        private: Registry() {};
      
        private: static RegistryKey classesRoot;
        private: static RegistryKey localMachine;
        private: static RegistryKey currentUser;
      
        public: static const RegistryKey& get_ClassesRoot() { return classesRoot; }
        public: static const RegistryKey& get_LocalMachine() { return localMachine; }
        public: static const RegistryKey& get_CurrentUser() { return currentUser; }
    };
    
    __declspec(selectany) RegistryKey Registry::classesRoot(HKEY_CLASSES_ROOT);
    __declspec(selectany) RegistryKey Registry::localMachine(HKEY_LOCAL_MACHINE);
    __declspec(selectany) RegistryKey Registry::currentUser(HKEY_CURRENT_USER);
}
