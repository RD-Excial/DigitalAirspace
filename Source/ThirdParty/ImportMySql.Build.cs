// Copyright 2015-2023 UEJoy �Զ������ļ� Version 1.1.0

using UnrealBuildTool;
using System.IO;

public class ImportMySql : ModuleRules
{
    public ImportMySql(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        //���ļ�·��
        PublicSystemLibraryPaths.Add(Path.Combine(ModuleDirectory, "lib"));
        //ͷ�ļ�·��
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        //��Ӿ�̬��
        PublicSystemLibraries.Add(Path.Combine(ModuleDirectory, "lib", "mysqlcppconn-static.lib"));
        // //���붯̬��
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "lib", "libcrypto-1_1-x64.dll"));
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "lib", "libssl-1_1-x64.dll"));
    }
}