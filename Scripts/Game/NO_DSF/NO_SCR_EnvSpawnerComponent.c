[ComponentEditorProps(category: "GameScripted/Coop", description: "Allows spawning of AI groups.")]
class NO_SCR_EnvSpawnerComponentClass : ScriptComponentClass
{
}

void ScriptInvoker_OnSpawnerEmptyDelegateEnv();
typedef func ScriptInvoker_OnSpawnerEmptyDelegateEnv;
typedef ScriptInvokerBase<ScriptInvoker_OnSpawnerEmptyDelegateEnv> ScriptInvoker_OnSpawnerEmptyEnv;


//------------------------------------------------------------------------------------------------
class NO_SCR_EnvSpawnerComponent : ScriptComponent
{
	
	
	
	[Attribute()]
	protected ref array<ref CountSpawnsByOnlinePlayer> m_rnDefaultPrefabs;
	
	[Attribute("0", UIWidgets.CheckBox, "If checked, respawn when gone or died. Does not work always")]
	protected bool m_bRespawn;	
	
	[Attribute("1", UIWidgets.Slider, "Only if respawn is enabled: How much time should be waitet between the check if the env is gone?", "1 100 1")]
	protected int m_bRespawnCheckTime;
	
	protected bool m_bRespawnFunctionAlreadyStarted = false;
	protected bool m_bIsAlive = false;
	
	[Attribute("0", UIWidgets.CheckBox, "If checked, spawns immediately.")]
	protected bool m_bSpawnImmediately;
	
	
	[Attribute("0", UIWidgets.Slider, "Radius that one prefab does occupied. Needed when there are multiple prefabs in there.", "0.5 100 0.5")]
	protected float m_fOccupiedSpace;	
	
	
	[Attribute("0", UIWidgets.Slider, "Radius in which should get searched for free place. Needed when there are multiple prefabs in there.", "10 1000 10")]
	protected float m_fFreeSpaceRadios;
	
	[Attribute(desc: "Define the spawns.",category: "Enable Dynamic Environment")]
	protected ref array<ref CountSpawnsByOnlinePlayerWithManagerEnv> m_rDynamicSpawns;
	
	[Attribute("0", UIWidgets.CheckBox, category: "Enable Dynamic Environment", desc: "Enable dynamic env")]
	protected bool m_bEnableDynamicEnv;
	
	IEntity Owner;
	

	vector parentVector[4]
	// Attached component.
	protected RplComponent m_pRplComponent;
	
	NO_SCR_SpawnManager spawnManager;

	//! Spawned enviroment relevant to the authority only.
	protected ref array<IEntity> m_pSpawnedEnvs = new array<IEntity>();
	
	//! Invoker which we can hook onto - see typedef above
	protected ref ScriptInvoker_OnSpawnerEmptyEnv m_pOnEmptyInvokerEnvSpawner = new ScriptInvoker_OnSpawnerEmptyEnv();
	
	
	array<IEntity> GetSpawnedenviroment()
	{
		return m_pSpawnedEnvs;
	}	
	
	void RemoveSpawned()
	{
		
		if(m_pSpawnedEnvs.Count() > 0)
		{
			foreach (IEntity spawned : m_pSpawnedEnvs)
			{
				RplComponent.DeleteRplEntity(spawned, false);
			}
			
			m_pSpawnedEnvs = new array<IEntity>();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker_OnSpawnerEmptyEnv GetOnEmptyInvoker()
	{
		return m_pOnEmptyInvokerEnvSpawner;
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	bool DoSpawn()
	{
		if (IsSpawned())
		{
			Print("NO_SCR_EnvSpawnerComponent cannot spawn group; group was spawned already!");
			return false;
		}

		if (!VerifyRplComponentPresent())
		{
			Print("NO_SCR_EnvSpawnerComponent cannot spawn group, spawner has no RplComponent!");
			return false;
		}

		if (!m_pRplComponent.IsMaster())
		{
			Print("NO_SCR_EnvSpawnerComponent caught non-master request to spawn!");
			return false;
		}

		auto game = GetGame();
		if(!game) return false;
		
		auto playerManager = game.GetPlayerManager();
		if(!playerManager) return false;
		
		auto world = GetOwner().GetWorld();
		if(!world) return false;
		
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		CountSpawnsByOnlinePlayer toUse;
		
		ref array<ResourceName> prefabs;
		
		if(!m_bEnableDynamicEnv)
		{
			
			foreach (CountSpawnsByOnlinePlayer spawn : m_rnDefaultPrefabs)
			{
				if(spawn.CountOfPlayers<=players.Count())
				{
					prefabs = spawn.prefab;
				}
			}
			
		}else{
			prefabs = new array<ResourceName>();
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
				
			foreach (CountSpawnsByOnlinePlayerWithManagerEnv spawn : m_rDynamicSpawns)
			{
				foreach(EnvType tmpEnvType : spawn.prefab)
					if(tmpEnvType == EnvType.CustomEnvironment1)
					{
						prefabs.Insert(factionToUse.CustomEnvironment1);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment2)
					{
						prefabs.Insert(factionToUse.CustomEnvironment2);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment3)
					{
						prefabs.Insert(factionToUse.CustomEnvironment3);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment4)
					{
						prefabs.Insert(factionToUse.CustomEnvironment4);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment5)
					{
						prefabs.Insert(factionToUse.CustomEnvironment5);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment6)
					{
						prefabs.Insert(factionToUse.CustomEnvironment6);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment7)
					{
						prefabs.Insert(factionToUse.CustomEnvironment7);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment8)
					{
						prefabs.Insert(factionToUse.CustomEnvironment8);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment9)
					{
						prefabs.Insert(factionToUse.CustomEnvironment9);
					}
					else if(tmpEnvType == EnvType.CustomEnvironment10)
					{
						prefabs.Insert(factionToUse.CustomEnvironment10);
					}
			}
			
		}
		
		
		
		
		foreach (ResourceName ressourceName : prefabs)
		{
			Resource enviromentPrefab = Resource.Load(ressourceName);
			if (!enviromentPrefab)
			{
				Print(string.Format("NO_SCR_EnvSpawnerComponent could not load '%1'", enviromentPrefab));
				return false;
			}
	
		
			
			vector pos;			
			SCR_WorldTools.FindEmptyTerrainPosition(pos, parentVector[3], m_fFreeSpaceRadios,m_fOccupiedSpace,2,TraceFlags.ENTS);
			parentVector[3]	= pos;
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;	
			spawnParams.Transform = parentVector;	
			
			IEntity spawnedEntity = GetGame().SpawnEntityPrefab(enviromentPrefab, world, spawnParams);
			if (!spawnedEntity)
			{
				Print(string.Format("NO_SCR_EnvSpawnerComponent could not spawn '%1'", enviromentPrefab));
				return false;
			}
	
			IEntity enviroment = IEntity.Cast(spawnedEntity);
			if (!enviroment)
			{
				Print(string.Format("NO_SCR_EnvSpawnerComponent spawned entity '%1' that is not of IEntity type, deleting!", enviromentPrefab));
				RplComponent.DeleteRplEntity(spawnedEntity, false);
				return false;
			}
			

			// Store enviroment			
			m_pSpawnedEnvs.Insert(enviroment);

		}
		
		if(m_bRespawn && !m_bRespawnFunctionAlreadyStarted)
		{
			GetGame().GetCallqueue().CallLater(checkIfRepsawnIsNeeded,m_bRespawnCheckTime*1000, true);				
		}
	
		m_bIsAlive = true;
		return true;
	}

	void checkIfRepsawnIsNeeded()
	{
		if(!m_bIsAlive) return;
		m_bRespawnFunctionAlreadyStarted = true;
		if(m_pSpawnedEnvs.Count()>0)
		{
			
			foreach(IEntity entity : m_pSpawnedEnvs)
			{
				if(entity != null)
				{
					auto DamageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
					if(DamageManager && !NO_SCR_DfsStatics.IsAlive(entity))
					{
						continue;
					}
					//means one entity is either alive or not null so no repsawn needed
					return;
				}
				
			}
		}
		
		m_pSpawnedEnvs = new array<IEntity>();
		m_bIsAlive = false;
		
		DoSpawnDefault();
	}
	

	
	//------------------------------------------------------------------------------------------------
	bool IsSpawned()
	{
		return m_pSpawnedEnvs.Count()>0;
	}

	//------------------------------------------------------------------------------------------------
	bool DoSpawnDefault()
	{
		return DoSpawn();
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	//protected override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
	//{
	//	super._WB_AfterWorldUpdate(owner, timeSlice);
	//	
	//	vector spawnPosition = GetSpawnPosition();
	//	Shape shape = Shape.CreateSphere(COLOR_YELLOW, ShapeFlags.ONCE | ShapeFlags.NOOUTLINE, spawnPosition, 0.3);
	//	Shape arrow = Shape.CreateArrow(GetOwner().GetOrigin(), spawnPosition, 0.1, COLOR_YELLOW, ShapeFlags.ONCE);
	//}
#endif

	//------------------------------------------------------------------------------------------------
	protected bool VerifyRplComponentPresent()
	{
		if (!m_pRplComponent)
		{
			Print("NO_SCR_EnvSpawnerComponent does not have a RplComponent attached!");
			return false;
		}

		return true;
	}
	
	//Todo not copy past that function make static xD
	//Thanks to narcoleptic marshmallow for his message on the arma discord: https://discord.com/channels/105462288051380224/976155351999201390/978395568453865622 
	private void GetAllChildren(IEntity parent, notnull inout array<IEntity> allChildren)
    {
        if (!parent)
            return;
        
        IEntity child = parent.GetChildren();
        
        while (child)
        {
            allChildren.Insert(child);
            child = child.GetSibling();
        }
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
		
		
		if(m_bEnableDynamicEnv)
			spawnManager =  GetSpawnManager();
	}

	//------------------------------------------------------------------------------------------------
	void NO_SCR_EnvSpawnerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~NO_SCR_EnvSpawnerComponent()
	{
	}

}

[BaseContainerProps(), BaseContainerCustomTitleField("CountOfPlayers")]
//! Count spawns by online player with manager
class CountSpawnsByOnlinePlayerWithManagerEnv
{
	//! How many player are joined?
	[Attribute("", UIWidgets.Slider, "How many player are joined?", "1 100 1")]
	int CountOfPlayers;
	
	//! Which group to spawn
	[Attribute("0",UIWidgets.ComboBox, "Which group to spawn","", ParamEnumArray.FromEnum(EnvType),category: "Spawn Manager")]
	ref array<EnvType> prefab;
}