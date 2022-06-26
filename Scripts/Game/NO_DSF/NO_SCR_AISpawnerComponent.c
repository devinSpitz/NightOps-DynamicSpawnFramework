[ComponentEditorProps(category: "GameScripted/Coop", description: "Allows spawning of AI groups.")]
class NO_SCR_AISpawnerComponentClass : ScriptComponentClass
{
}

void ScriptInvoker_OnSpawnerEmptyDelegate();
typedef func ScriptInvoker_OnSpawnerEmptyDelegate;
typedef ScriptInvokerBase<ScriptInvoker_OnSpawnerEmptyDelegate> ScriptInvoker_OnSpawnerEmpty;

//------------------------------------------------------------------------------------------------
class NO_SCR_AISpawnerComponent : ScriptComponent
{	
	
	[Attribute("0", UIWidgets.CheckBox, "If checked, spawns group immediately FYI: if you change the faction on the spawn manager while inGame the already spawned AI's will not respawn as new faction.",category: "Spawn Manager")]
	protected bool m_bSpawnImmediately;
	
	[Attribute("0", UIWidgets.CheckBox, "If checked, respawn when died.",category: "Spawn Manager")]
	protected bool m_bRespawn;
	[Attribute("0", UIWidgets.CheckBox, "Stack all selection until player count.",category: "Spawn Manager")]
	protected bool m_bShouldStack;
	
	[Attribute("5", UIWidgets.Slider, "Negative spawn offset on the Z axis so that mutliple Ais do not spawn in each other.", "5 1000 1",category: "Spawn Manager")]
	protected float m_fNegativeZOffset;

	[Attribute(category: "Spawn Manager", desc: "Define the spawns teams.")]
	protected ref array<ref CountSpawnsByOnlinePlayerWithManager> m_rSpawns;
	
	[Attribute("0", UIWidgets.ComboBox, "Override any select on this spawn with this team","", ParamEnumArray.FromEnum(AiGroupType),category: "Spawn Manager Team Override")]
	protected AiGroupType m_eAiGroupType;	
	
	IEntity Owner;

	NO_SCR_SpawnManager spawnManager;
	
	vector parentVector[4]

	protected RplComponent m_pRplComponent;
	
	[Attribute(category: "Detaile Spawn (without spawn manager)", desc: "Only work when now spawn manager is available or m_rnIgnoreSpawnManagerAndUsePrefabs is set to true!")]
	protected ref array<ref CountSpawnsByOnlinePlayer> m_rnDefaultPrefabs;
	
	[Attribute("0", UIWidgets.CheckBox, category: "Detaile Spawn (without spawn manager)", desc: "Ignore the Spawn Manager")]
	protected bool m_bIgnoreSpawnManagerAndUsePrefabs;
	
	
	
	//! Spawned agent relevant to the authority only.
	protected ref array<AIAgent> m_pSpawnedAgents = new array<AIAgent>();
	
	//! only triggeres when respawn is enabled
	protected ref ScriptInvoker_OnSpawnerEmpty m_pOnEmptyInvokerNightOpsAiSpawner = new ScriptInvoker_OnSpawnerEmpty();
	
	//! get the spawned agent
	array<AIAgent> GetSpawnedAgent()
	{
		return m_pSpawnedAgents;
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker_OnSpawnerEmpty GetOnEmptyInvoker()
	{
		return m_pOnEmptyInvokerNightOpsAiSpawner;
	}

	
	//! Will delete the spawned prefab
	void RemoveSpawned()
	{
		if(m_pSpawnedAgents.Count() > 0)
		{
			foreach (AIAgent spawned : m_pSpawnedAgents)
			{
				RplComponent.DeleteRplEntity(spawned, false);
			}
			
			m_pSpawnedAgents = new array<AIAgent>();
		}
	}
	
	//! Will directly spawned the prefab depending the settings
	bool DoSpawn()
	{
		if (IsSpawned())
		{
			Print("NO_SCR_AISpawnerComponent cannot spawn group; group was spawned already!");
			return false;
		}

		if (!VerifyRplComponentPresent())
		{
			Print("NO_SCR_AISpawnerComponent cannot spawn group, spawner has no RplComponent!");
			return false;
		}

		if (!m_pRplComponent.IsMaster())
		{
			Print("NO_SCR_AISpawnerComponent caught non-master request to spawn!");
			return false;
		}
		auto game = GetGame();
		if(!game) return false;
		
		auto playerManager = game.GetPlayerManager();
		if(!playerManager) return false;
		
		BaseWorld world = Owner.GetWorld();
		if(!world) return false;
		
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		ref array<ResourceName> ressourceNamesToSpawn = new array<ResourceName>();
		
		FactionReferences factionToUse;
		IEntity parent = Owner.GetParent();
		if(parent)
		{
			NO_SCR_SpawnTrigger spawnTrigger = NO_SCR_SpawnTrigger.Cast(parent);
			if(spawnTrigger && spawnManager)
			{
				foreach(FactionReferences factionReference : spawnManager.m_rFactionPrefabs)
				{
					if(factionReference.m_faction==spawnManager.m_factionToSpawnWhenDynamicFaction)
					{
						factionToUse = factionReference;
					}
				}
			}
		}
		
		
		if(!spawnManager)
		{
			
			ref array<ResourceName> tmpRessourceNamesToSpawn = new array<ResourceName>();
			CountSpawnsByOnlinePlayer toUse;
			foreach (CountSpawnsByOnlinePlayer spawn : m_rnDefaultPrefabs)
			{
				if(spawn.CountOfPlayers<=players.Count())
				{
					if(m_bShouldStack)
					{
						tmpRessourceNamesToSpawn.InsertAll(spawn.prefab);
					}
					else{
						toUse = spawn;
					}
					
				}
			}
			if(!m_bShouldStack)
				tmpRessourceNamesToSpawn = toUse.prefab;
			
			ressourceNamesToSpawn = tmpRessourceNamesToSpawn;
		}
		else if(spawnManager && m_eAiGroupType != AiGroupType.PleaseSelect) // overwrite all teams
		{
			CountSpawnsByOnlinePlayerWithManager toUse;
			foreach (CountSpawnsByOnlinePlayerWithManager spawn : m_rSpawns)
			{
				if(spawn!=null && spawn.CountOfPlayers<=players.Count())
				{
					if(!m_bShouldStack)
					{
						ressourceNamesToSpawn = new ref array<ResourceName>();
					}
					
					foreach(AiGroupType tmpAiGroupType : spawn.prefab)
						if(m_eAiGroupType == AiGroupType.FireTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.FireTeam);
						}
						else if(m_eAiGroupType == AiGroupType.LightFireTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.LightFireTeam);
						}
						else if(m_eAiGroupType == AiGroupType.MachineGunTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.MachineGunTeam);
						}
						else if(m_eAiGroupType == AiGroupType.RifleSquad)
						{
							ressourceNamesToSpawn.Insert(factionToUse.RifleSquad);
						}
						else if(m_eAiGroupType == AiGroupType.SentryTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SentryTeam);
						}
						else if(m_eAiGroupType == AiGroupType.SniperTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SniperOrManeuverTeam);
						}
						else if(m_eAiGroupType == AiGroupType.GLTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.GLTeam);
						}
						else if(m_eAiGroupType == AiGroupType.LATTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.LatOrAtTeam);
						}
						else if(m_eAiGroupType == AiGroupType.SuppressTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SuppressTeam);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam1)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam1);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam2)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam2);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam3)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam3);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam4)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam4);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam5)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam5);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam6)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam6);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam7)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam7);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam8)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam8);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam9)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam9);
						}
						else if(m_eAiGroupType == AiGroupType.CustomTeam10)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam10);
						}
					
				}
			}
		}
		else if(spawnManager && m_eAiGroupType == AiGroupType.PleaseSelect)
		{
			CountSpawnsByOnlinePlayerWithManager toUse;
			foreach (CountSpawnsByOnlinePlayerWithManager spawn : m_rSpawns)
			{
				if(spawn!=null && spawn.CountOfPlayers<=players.Count())
				{
					if(!m_bShouldStack)
					{
						ressourceNamesToSpawn = new ref array<ResourceName>();
					}
					
					foreach(AiGroupType tmpAiGroupType : spawn.prefab)
						if(tmpAiGroupType == AiGroupType.FireTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.FireTeam);
						}
						else if(tmpAiGroupType == AiGroupType.LightFireTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.LightFireTeam);
						}
						else if(tmpAiGroupType == AiGroupType.MachineGunTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.MachineGunTeam);
						}
						else if(tmpAiGroupType == AiGroupType.RifleSquad)
						{
							ressourceNamesToSpawn.Insert(factionToUse.RifleSquad);
						}
						else if(tmpAiGroupType == AiGroupType.SentryTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SentryTeam);
						}
						else if(tmpAiGroupType == AiGroupType.SniperTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SniperOrManeuverTeam);
						}
						else if(tmpAiGroupType == AiGroupType.GLTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.GLTeam);
						}
						else if(tmpAiGroupType == AiGroupType.LATTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.LatOrAtTeam);
						}
						else if(tmpAiGroupType == AiGroupType.SuppressTeam)
						{
							ressourceNamesToSpawn.Insert(factionToUse.SuppressTeam);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam1)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam1);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam2)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam2);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam3)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam3);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam4)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam4);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam5)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam5);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam6)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam6);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam7)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam7);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam8)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam8);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam9)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam9);
						}
						else if(tmpAiGroupType == AiGroupType.CustomTeam10)
						{
							ressourceNamesToSpawn.Insert(factionToUse.CustomTeam10);
						}
				}
			}
		}
		
		
		bool first = true;	
		foreach (ResourceName ressourceName : ressourceNamesToSpawn)
		{
			Resource agentPrefab = Resource.Load(ressourceName);
			if (!agentPrefab)
			{
				Print(string.Format("NO_SCR_AISpawnerComponent could not load '%1'", agentPrefab));
				return false;
			}
		

			vector tmp = parentVector[3];
			vector newPos;
			if(first)
			{
			 	newPos = Vector(tmp[0], tmp[1], tmp[2]);
			}
			else
			{
			 	newPos = Vector(tmp[0], tmp[1], tmp[2]+m_fNegativeZOffset);
			}
			vector pos;			
			SCR_WorldTools.FindEmptyTerrainPosition(pos,newPos , 10,2,2,TraceFlags.VISIBILITY);
			parentVector[3]	= pos;
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;	
			spawnParams.Transform = parentVector;	
						
			IEntity spawnedEntity = game.SpawnEntityPrefab(agentPrefab, world, spawnParams);
			if (!spawnedEntity)
			{
				Print(string.Format("NO_SCR_AISpawnerComponent could not spawn '%1'", agentPrefab));
				return false;
			}
		
			AIAgent agent = AIAgent.Cast(spawnedEntity);
			if (!agent)
			{
				Print(string.Format("NO_SCR_AISpawnerComponent spawned entity '%1' that is not of AIAgent type, deleting!", agentPrefab));
				RplComponent.DeleteRplEntity(spawnedEntity, false);
				return false;
			}
		
				// Store agent
			m_pSpawnedAgents.Insert(agent);
				
				
			
			NO_SCR_DfsStatics.AddWaypointsToAi(agent, Owner);
				
				
			//respawn
			if(m_bRespawn)
			{
				SCR_AIGroup aiGroup = SCR_AIGroup.Cast(agent);
				if (aiGroup)
				{
					aiGroup.GetOnEmpty().Insert(OnEmpty);
				}
			}
		
			first = false;
		}
		
		return true;
	}
	
	
	
	
	//! Will get triggered when respawn is active and the Ai has died
	protected event void OnEmpty()
	{
		m_pOnEmptyInvokerNightOpsAiSpawner.Invoke();
		m_pSpawnedAgents = new array<AIAgent>();
		DoSpawnDefault();
		
	}

	//! return if the AI already has spawned
	bool IsSpawned()
	{
		return m_pSpawnedAgents.Count()>0;
	}

	//! Actives the spawn and gives back if it realy did happen
	bool DoSpawnDefault()
	{
		return DoSpawn();
	}
	

	//------------------------------------------------------------------------------------------------
	protected bool VerifyRplComponentPresent()
	{
		if (!m_pRplComponent)
		{
			Print("NO_SCR_AISpawnerComponent does not have a RplComponent attached!");
			return false;
		}

		return true;
	}
	

	//! Init function
	override void OnPostInit(IEntity owner)
	{
		Owner = owner;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!VerifyRplComponentPresent())
			return;

		SetEventMask(owner, EntityEvent.INIT);
	}

	//! Init function
	override void EOnInit(IEntity owner)
	{

		owner.GetTransform(parentVector);
		
		if (m_bSpawnImmediately)
		{
			// Spawning of Replicated items must not happen in EOnInit,
			// we delay the call to happen asap (after EOnInit)
			GetGame().GetCallqueue().CallLater(DoSpawnDefault, 0);
		}
		
		if(!m_bIgnoreSpawnManagerAndUsePrefabs)
			spawnManager =  GetSpawnManager();
		
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{		
		auto allAgents = GetSpawnedAgent();
				
		foreach (AIAgent agent : allAgents)
		{
			SCR_AIGroup aiGroup = SCR_AIGroup.Cast(agent);
			if (aiGroup)
			{
				aiGroup.GetOnEmpty().Remove(OnEmpty);
			}
		}
		

	}

	//------------------------------------------------------------------------------------------------
	void NO_SCR_AISpawnerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}


	//------------------------------------------------------------------------------------------------
	void ~NO_SCR_AISpawnerComponent()
	{
	}
}




[BaseContainerProps(), BaseContainerCustomTitleField("CountOfPlayers")]
//! Count spawns by online player
class CountSpawnsByOnlinePlayer
{
	//! How many player are joined?
	[Attribute("", UIWidgets.Slider, "How many player are joined?", "1 100 1")]
	int CountOfPlayers;
	
	//! Group prefabs to spawn.
	[Attribute("", UIWidgets.Auto, "Group prefabs to spawn.")]
	ref array<ResourceName> prefab;
}

[BaseContainerProps(), BaseContainerCustomTitleField("CountOfPlayers")]
//! Count spawns by online player with manager
class CountSpawnsByOnlinePlayerWithManager
{
	//! How many player are joined?
	[Attribute("", UIWidgets.Slider, "How many player are joined?", "1 100 1")]
	int CountOfPlayers;
	
	//! Which group to spawn
	[Attribute("0",UIWidgets.ComboBox, "Which group to spawn","", ParamEnumArray.FromEnum(AiGroupType),category: "Spawn Manager")]
	ref array<AiGroupType> prefab;
}

