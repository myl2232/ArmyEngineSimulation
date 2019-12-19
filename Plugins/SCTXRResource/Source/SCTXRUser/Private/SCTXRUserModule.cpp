#include "SCTXRUserModule.h"

IMPLEMENT_MODULE(FSCTXRUserModule, SCTXRUser);

FSCTXRUserModule* FSCTXRUserModule::Singleton = NULL;

void FSCTXRUserModule::StartupModule()
{
    Singleton = this;
}

void FSCTXRUserModule::ShutdownModule()
{
}

SCTXRUSER_API FSCTXRUserModule& FSCTXRUserModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FSCTXRUserModule>("SCTXRUser");
    }
    check(Singleton != NULL);

    return *Singleton;
}

//TSharedPtr<FSCTXRUserModule> FSCTXRUserModule::GetUserManager()
//{
//    if (!UserManager.IsValid())
//    {
//        UserManager = MakeShareable(new FSCTXRUserModule());
//    }
//
//    return UserManager;
//}
