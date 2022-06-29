

NO_SCR_SpawnManager g_SpawnManagerInstance;
NO_SCR_SpawnManager GetSpawnManager()
{
	return g_SpawnManagerInstance;
};

[EntityEditorProps(category: "GameScripted/Components", description: "Faction group reference list holder Component")]
class NO_SCR_SpawnManagerClass: GenericEntityClass
{
	
};

class NO_SCR_SpawnManager : GenericEntity
{
	[Attribute()]
	ref array<ref FactionReferences> m_rFactionPrefabs;
	
	[Attribute("USSR", UIWidgets.EditBox, "Faction to be spawned")]
	FactionKey m_factionToSpawnWhenDynamicFaction;	
	
	void NO_SCR_SpawnManager(IEntitySource src, IEntity parent)
	{
		if (!g_SpawnManagerInstance)
		{
			g_SpawnManagerInstance = this;
		}
		else{
			Debug.Error("Multiple Spawn Manager are alive that will cause problems.");
		}
		
	#ifdef ENABLE_DIAG
		DiagMenu.RegisterMenu(SCR_DebugMenuID.DEBUGUI_TASKS_MENU, "Tasks", "");
		DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_EXECUTE_TASKS, "", "Execute tasks", "Tasks", true);
		DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_SHOW_ALL_TASKS, "", "Print all tasks", "Tasks", false);
	#endif
		SetEventMask(EntityEvent.FRAME | EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
		SetFlags(EntityFlags.NO_LINK, false);
		
	}
	
}



[BaseContainerProps(), BaseContainerCustomTitleField("m_faction")]
class FactionReferences
{
	
	[Attribute("USSR", UIWidgets.EditBox, "Which faction you want to define?")]
	FactionKey m_faction;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName FireTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName LightFireTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName MachineGunTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName RifleSquad;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName SentryTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName SniperOrManeuverTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName GLTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName LatOrAtTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName SuppressTeam;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam1;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam2;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam3;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam4;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam5;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam6;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam7;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName 	CustomTeam8;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam9;
	[Attribute("", UIWidgets.Auto, "Group prefab to spawn.")]
	ResourceName CustomTeam10;
	
	
	
	

	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment1;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment2;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment3;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment4;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment5;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment6;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment7;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName 	CustomEnvironment8;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment9;
	[Attribute("", UIWidgets.Auto, "Environment prefab to spawn.")]
	ResourceName CustomEnvironment10;
}