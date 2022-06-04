[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that Spawns Environment or AI stuff.")]
class NO_SCR_SpawnTriggerClass: SCR_BaseTriggerEntityClass
{
};

class NO_SCR_SpawnTrigger : SCR_BaseTriggerEntity
{
	[Attribute("0", UIWidgets.CheckBox, "If checked, delete/Despawn when no player is in Trigger!")]
	protected bool m_bDelete;	
	[Attribute("0", UIWidgets.CheckBox, "Update Navmesh when spawning?")]
	protected bool updateNavmesh;	
	
	
	[Attribute("0", UIWidgets.CheckBox, "Check if the AI spawns should be randomized!")]
	protected bool m_bRandomizedSpawns;	
	
	[Attribute("0", UIWidgets.Slider, "How many percentige of the AI spawns should be populated (only when RandomizedSpawns are enabled)", "0 100 1")]
	protected int PercentageAi;

	[Attribute("USSR", UIWidgets.EditBox, "Faction")]
	FactionKey m_faction;
		
	BaseGameMode GameMode;
	bool alreadySpawned = false;
	IEntity Owner;
	ArmaReforgerScripted GameSingleEntity;
	private RplComponent m_pRplComponent;
	protected int m_iCount = 0; // keep count of enemies
	
	
	ref array<IEntity> children = new array<IEntity>();
	ref array<NO_SCR_AISpawnerComponent> childrenAiSpawner =  new array<NO_SCR_AISpawnerComponent>;
	ref array<NO_SCR_EnvSpawnerComponent> childrenEnvSpawner = new array<NO_SCR_EnvSpawnerComponent>();
	
	override void OnInit(IEntity owner)
	{
		Owner = owner;
		super.OnInit(owner);
		if (!GetGame().InPlayMode())
            return;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if(!m_pRplComponent) Debug.Error("NO_SCR_SpawnTrigger cannot hook to the RplComponent please add one!");
		
		
		if(m_pRplComponent.IsMaster())
		
		
		GameSingleEntity =  GetGame();
		GameMode = GameSingleEntity.GetGameMode();
		NO_SCR_DfsStatics.GetAllChildren(this,children);
		//Get components we need
		foreach (int i, IEntity child : children)
		{	
			ref array<IEntity> childrenTmp = new array<IEntity>();
			auto ai = NO_SCR_DfsStatics.GetAiFromEnitity(child,childrenTmp);
			if(ai)
				childrenAiSpawner.Insert(ai);
			auto env = NO_SCR_EnvSpawnerComponent.Cast(child.FindComponent(NO_SCR_EnvSpawnerComponent));
			if(env)
				childrenEnvSpawner.Insert(env);
		}
		
		
	}
	

	
	//Thanks to Herbiie for his code and the wiki: https://github.com/Herbiie/ArmAReforgerMissionMakingGuide
    // Set up the filter
    override bool ScriptedEntityFilterForQuery(IEntity ent) {
        SCR_ChimeraCharacter cc = SCR_ChimeraCharacter.Cast(ent);
        if (!cc) return false; // If the entity is not a person, filter it out
        if (cc.GetFactionKey() != m_faction) return false; // If the entity does not have the Faction key of USSR, filter it out
        if (!IsAlive(cc)) return false; // If the entity is dead, filter it out
        return true; // Otherwise, include it!
    }
 
    override void OnActivate(IEntity ent)
    {
        ++m_iCount; // When activated (i.e. when an alive USSR soldier entity enters), add 1 to the number m_iCount
		if(m_iCount==1&&m_pRplComponent.IsMaster() && GameMode.IsLoaded()) Spawn(); // otherwise it should already be spawn
    }
 
    override void OnDeactivate(IEntity ent)
    {        
		
        --m_iCount; // When deactivated (i.e. if the soldier leaves or dies) take away 1 to the number m_iCount
		if(m_iCount==0 && m_pRplComponent.IsMaster()&&m_bDelete && GameMode.IsLoaded()) 
		{
			Despawn();
			//todo if master maybe delete?
		}
    }

	
	void Despawn()
	{
		if(!m_pRplComponent.IsMaster()) return;
		
		if(!children || children.Count()<=0) 
		{
			return; // nothing to spawn
		}
		
		foreach (int i, NO_SCR_AISpawnerComponent ai : childrenAiSpawner)
		{	
			ai.RemoveSpawned();
		}		
		foreach (int i, NO_SCR_EnvSpawnerComponent env : childrenEnvSpawner)
		{	
			env.RemoveSpawned();
		}
		//Future
		
	}	
	
	void Spawn()
	{
				
		if(!m_pRplComponent.IsMaster()) return;
		
		if(!children || children.Count()<=0) 
		{
			return; // nothing to spawn
		}
		
		
		alreadySpawned = true;
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GameSingleEntity.GetAIWorld());
		
		array<NO_SCR_AISpawnerComponent> aisToSpawn = new array<NO_SCR_AISpawnerComponent>();
		aisToSpawn.Copy(childrenAiSpawner);
		if(m_bRandomizedSpawns && PercentageAi<100)
		{
			
			array<NO_SCR_AISpawnerComponent> aisToSpawnTmp =  new array<NO_SCR_AISpawnerComponent>;
			int howMany = Math.Round((aisToSpawn.Count()/100)*PercentageAi);
			
			for(int i = 0; i < howMany; ++i)
			{
				auto newIndex = aisToSpawn.GetRandomIndex();
				aisToSpawnTmp.Insert(aisToSpawn[newIndex]);
				aisToSpawn.Remove(newIndex);
			}
			
			aisToSpawn = aisToSpawnTmp;
				
		}
		
		
		
		foreach (int i, NO_SCR_AISpawnerComponent ai : aisToSpawn)
		{	
			SpawnAi(ai);
		}		
		foreach (int i, NO_SCR_EnvSpawnerComponent env : childrenEnvSpawner)
		{	
			SpawnEnv(env);
		}
				
		if (updateNavmesh && aiWorld)
		{
			foreach (NO_SCR_EnvSpawnerComponent env : childrenEnvSpawner)
			{	
				foreach (IEntity envSingle : env.GetSpawnedenviroment())
				{
					array<ref Tuple2<vector, vector>> areas = new array<ref Tuple2<vector, vector>>; //--- Min, max
					aiWorld.GetNavmeshRebuildAreas(envSingle, areas);
					GameSingleEntity.GetCallqueue().CallLater(aiWorld.RequestNavmeshRebuildAreas, 1000, false, areas); //--- Called *before* the entity is deleted with a delay, ensures the regeneration doesn't accidentaly get anything from the entity prior to full destruction
				}
			}
		}

		
		
	}
	
	void SpawnAi(NO_SCR_AISpawnerComponent ai)
	{
		ai.DoSpawnDefault();
	}
	
	void SpawnEnv(NO_SCR_EnvSpawnerComponent env)
	{
		env.DoSpawnDefault();
		//Todo Gen navMesh
	}
	
 
}
