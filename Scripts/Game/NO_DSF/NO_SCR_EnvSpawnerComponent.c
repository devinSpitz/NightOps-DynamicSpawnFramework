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
	
	
	[Attribute("0", UIWidgets.CheckBox, "If checked, spawns immediately.")]
	protected bool m_bSpawnImmediately;
	
	
	[Attribute("0", UIWidgets.Slider, "Radius that one prefab does occupied. Needed when there are multiple prefabs in there.", "0.5 100 0.5")]
	protected float m_fOccupiedSpace;	
	
	
	[Attribute("0", UIWidgets.Slider, "Radius in which should get searched for free place. Needed when there are multiple prefabs in there.", "10 1000 10")]
	protected float m_fFreeSpaceRadios;

	vector parentVector[4]
	// Attached component.
	protected RplComponent m_pRplComponent;
	

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
		
		foreach (CountSpawnsByOnlinePlayer spawn : m_rnDefaultPrefabs)
		{
			if(spawn.CountOfPlayers<=players.Count())
			{
				toUse = spawn;
			}
		}
		
		foreach (ResourceName ressourceName : toUse.prefab)
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
		return true;
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
	void NO_SCR_EnvSpawnerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~NO_SCR_EnvSpawnerComponent()
	{
	}

}
