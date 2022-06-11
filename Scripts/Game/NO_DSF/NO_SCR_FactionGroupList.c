[EntityEditorProps(category: "GameScripted/Components", description: "Faction group reference list holder Component")]
class NO_SCR_FactionGroupListComponentClass: ScriptComponentClass
{
	
};


class NO_SCR_FactionGroupListComponent : ScriptComponent
{
	[Attribute()]
	ref array<ref FactionReferences> m_rFactionPrefabs;
	
	[Attribute("USSR", UIWidgets.EditBox, "Faction to be spawned. Only needed when m_bShouldUseDynamicFaction is set on the trigger!")]
	FactionKey m_factionToSpawnWhenDynamicFaction;	
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
}