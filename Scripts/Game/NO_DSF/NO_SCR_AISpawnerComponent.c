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
	[Attribute("0", UIWidgets.CheckBox, "If checked, spawns group immediately.")]
	protected bool m_bSpawnImmediately;

	[Attribute("0", UIWidgets.CheckBox, "If checked, respawn when died.")]
	protected bool m_bRespawn;
	
	[Attribute("5", UIWidgets.Slider, "Negative spawn offset on the Z axis.", "5 1000 1")]
	protected float m_fNegativeZOffset;

	IEntity Owner;


	vector parentVector[4]
	// Attached component.
	protected RplComponent m_pRplComponent;

	[Attribute()]
	protected ref array<ref CountSpawnsByOnlinePlayer> m_rnDefaultPrefabs;
	
	//! Spawned agent relevant to the authority only.
	protected ref array<AIAgent> m_pSpawnedAgents = new array<AIAgent>();
	
	//! Invoker which we can hook onto - see typedef above
	protected ref ScriptInvoker_OnSpawnerEmpty m_pOnEmptyInvoker = new ScriptInvoker_OnSpawnerEmpty();
	
	
	array<AIAgent> GetSpawnedAgent()
	{
		return m_pSpawnedAgents;
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker_OnSpawnerEmpty GetOnEmptyInvoker()
	{
		return m_pOnEmptyInvoker;
	}

	
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
	
	//------------------------------------------------------------------------------------------------
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
		
		auto world = Owner.GetWorld();
		if(!world) return false;
		
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		CountSpawnsByOnlinePlayer toUse;
		
		foreach (CountSpawnsByOnlinePlayer spawn : m_rnDefaultPrefabs)
		{
			if(spawn.CountOfPlayers<=players.Count())
			{
				toUse = spawn;
			}
		}
		bool first = true;
		foreach (ResourceName ressourceName : toUse.prefab)
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
				
				
			ref array<IEntity> children = new array<IEntity>();
			NO_SCR_DfsStatics.GetAllChildren(Owner,children);
		
				
				
			if(!children || children.Count()<=0) 
			{
				return false;
			}
				
				
			AIWaypointCycle cycle;
			ref array<AIWaypoint> patrolWaypoints = new array<AIWaypoint>();
			SCR_DefendWaypoint defend;
			SCR_BoardingWaypoint onBoard;
			foreach (IEntity waypointEntity : children)
			{
				auto tmpWaypoint = SCR_AIWaypoint.Cast(waypointEntity);
				auto tmpCycle = AIWaypointCycle.Cast(waypointEntity);
			
				if(!defend)
				{
					defend = SCR_DefendWaypoint.Cast(waypointEntity);
					if(defend)
						continue;
				}
				if(!onBoard)
				{
					onBoard = SCR_BoardingWaypoint.Cast(waypointEntity);
					if(onBoard)
						continue;
				}
		
					
				if(tmpWaypoint &&  !tmpCycle) 
				{
					patrolWaypoints.Insert(tmpWaypoint);
				}
				if(tmpCycle) 
				{
					cycle = tmpCycle;
				}
			}
				
			if(cycle)
			{
				cycle.SetWaypoints(patrolWaypoints);	
					
			 	agent.AddWaypoint(cycle);
			}
			else
			{
				if(patrolWaypoints && patrolWaypoints.Count()>0)
				{
					foreach (AIWaypoint patrol : patrolWaypoints)
					{
						agent.AddWaypoint(patrol);
					}
				}
				if(defend)
				{
					agent.AddWaypoint(defend);
				}
				if(onBoard)
				{
					agent.AddWaypoint(onBoard);
				}
			}
				
					
				
				
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
	
	//------------------------------------------------------------------------------------------------
	protected event void OnEmpty()
	{
		m_pOnEmptyInvoker.Invoke();
		m_pSpawnedAgents = new array<AIAgent>();
		DoSpawnDefault();
		
	}

	//------------------------------------------------------------------------------------------------
	bool IsSpawned()
	{
		return m_pSpawnedAgents.Count()>0;
	}

	//------------------------------------------------------------------------------------------------
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
	

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		Owner = owner;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!VerifyRplComponentPresent())
			return;

		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{

		owner.GetTransform(parentVector);
		
		if (m_bSpawnImmediately)
		{
			// Spawning of Replicated items must not happen in EOnInit,
			// we delay the call to happen asap (after EOnInit)
			GetGame().GetCallqueue().CallLater(DoSpawnDefault, 0);
		}
		
		
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
class CountSpawnsByOnlinePlayer
{
	[Attribute("", UIWidgets.Slider, "How many player are joined?", "1 100 1")]
	int CountOfPlayers;
	
	[Attribute("", UIWidgets.Auto, "Group prefabs to spawn.")]
	ref array<ResourceName> prefab;
}