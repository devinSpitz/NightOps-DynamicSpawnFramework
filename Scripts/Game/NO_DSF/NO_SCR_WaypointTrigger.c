[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that redefine Waypoints of the AI")]
class NO_SCR_WaypointTriggerClass: SCR_BaseTriggerEntityClass
{
};

class NO_SCR_WaypointTrigger : SCR_BaseTriggerEntity
{

	[Attribute("USSR", UIWidgets.EditBox, "Faction which should Trigger",category: "Spawn Trigger")]
	FactionKey m_faction;

	[Attribute("0", UIWidgets.CheckBox, "Triggers by players only!",category: "Spawn Trigger")]
	bool m_bShouldTriggerOnlyOnPlayerContact;	
	

	BaseGameMode GameMode;
	IEntity Owner;
	ArmaReforgerScripted GameSingleEntity;
	private RplComponent m_pRplComponent;
	protected int m_iCount = 0; // keep count of enemies
	
	ref array<IEntity> children = new array<IEntity>();
	NO_SCR_AISpawnerComponent myAgent;
	bool isActivated = false;
	
	
	ref array<NO_SCR_WaypointTrigger> childrenTriggers = new array<NO_SCR_WaypointTrigger>();
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
		
		
		//if parent is aispawner set Ai Agent and activate yourself
		IEntity parent = GetParent();
		NO_SCR_AISpawnerComponent spawner = NO_SCR_AISpawnerComponent.Cast(parent.FindComponent(NO_SCR_AISpawnerComponent));
		if(spawner)
		{
			myAgent = spawner;
			isActivated = true;
		}
		
		
		NO_SCR_DfsStatics.GetAllChildren(this,children);
		//Get child triggers and set them in order
		foreach (int i, IEntity child : children)
		{	
			NO_SCR_WaypointTrigger trigger = NO_SCR_WaypointTrigger.Cast(child);
			if(trigger)
			{
				childrenTriggers.Insert(trigger);
				trigger.myAgent = myAgent;
			}
		}
		
		
	}
	

	
	//Thanks to Herbiie for his code and the wiki: https://github.com/Herbiie/ArmAReforgerMissionMakingGuide
    // Set up the filter
    override bool ScriptedEntityFilterForQuery(IEntity ent) {
		if(!isActivated) return false;
        SCR_ChimeraCharacter cc = SCR_ChimeraCharacter.Cast(ent);
        if (!cc) return false; // If the entity is not a person, filter it out
        if (cc.GetFactionKey() != m_faction) return false; // If the entity does not have the Faction key of USSR, filter it out
        if (!IsAlive(cc)) return false; // If the entity is dead, filter it out
		if(m_bShouldTriggerOnlyOnPlayerContact && !NO_SCR_DfsStatics.IsPlayer(ent,GameSingleEntity)) return false;
        return true; // Otherwise, include it!
    }
 

	
    override void OnActivate(IEntity ent)
    {
        ++m_iCount; // When activated (i.e. when an alive USSR soldier entity enters), add 1 to the number m_iCount
		if(m_iCount==1&&m_pRplComponent.IsMaster() && GameMode.IsLoaded()) 
		{
			//active the child triggers
			ActiveNow();

		}
    }

	
	void ActiveNow()
	{
		array<AIAgent> spawnedAgent = myAgent.GetSpawnedAgent();
		if(spawnedAgent.Count()<=0) return; // no more left
		foreach (int i, NO_SCR_WaypointTrigger child : childrenTriggers)
		{	
			child.isActivated= true;
		}
			
		//reset the waypojnts
		foreach (int i, AIAgent agent : spawnedAgent)
		{	
			
			array<AIWaypoint> waypoints = new array<AIWaypoint>;	
			agent.GetWaypoints(waypoints);
			foreach (AIWaypoint waypoint : waypoints)
			{	
				agent.RemoveWaypoint(waypoint);
			}
			agent.ClearOrders();
			NO_SCR_DfsStatics.AddWaypointsToAi(agent,Owner);
		}
	}

	
 
}
