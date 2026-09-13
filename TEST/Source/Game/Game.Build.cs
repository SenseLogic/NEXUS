using System.IO;
using Flax.Build;
using Flax.Build.NativeCpp;

public class Game : GameModule
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        BuildNativeCode = true;
    }

    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);

        options.ScriptingAPI.IgnoreMissingDocumentationWarnings = true;

        var nexusRefPath = Path.GetFullPath(Path.Combine(FolderPath, "../../../CODE/Nexus/Counted"));
        options.PublicIncludePaths.Add(nexusRefPath);
        AddSourceFileIfExists(options, Path.Combine(nexusRefPath, "CountedObject.cpp"));
        options.CompileEnv.CppVersion = CppVersion.Cpp17;
    }
}
