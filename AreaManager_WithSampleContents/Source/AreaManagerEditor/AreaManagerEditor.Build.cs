using UnrealBuildTool;

public class AreaManagerEditor : ModuleRules
{
	public AreaManagerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AreaManager",
				"Core",
				"CoreUObject",
				"EditorSubsystem",
				"Engine",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"EditorFramework",
				"Blutility",
				"ComponentVisualizers",
				"InputCore",
				"Projects",
				"RenderCore",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"UnrealEd",
				"UMG",
				"UMGEditor",
			}
		);
	}
}
