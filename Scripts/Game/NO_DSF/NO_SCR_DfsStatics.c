class NO_SCR_DfsStatics
{
	//Thanks to narcoleptic marshmallow for his message on the arma discord: https://discord.com/channels/105462288051380224/976155351999201390/978395568453865622 
	static void GetAllChildren(IEntity parent, notnull inout array<IEntity> allChildren)
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
 	
	static NO_SCR_AISpawnerComponent GetAiFromEnitity(IEntity entity,array<IEntity> childrenTmp)
	{
		auto ai = NO_SCR_AISpawnerComponent.Cast(entity.FindComponent(NO_SCR_AISpawnerComponent));
		if(ai) 
		{
			return ai;
		}
		GetAllChildren(entity,childrenTmp);
		if(childrenTmp && childrenTmp.Count()>=0) 
		{
			foreach (int i, IEntity childTmp : childrenTmp)
			{	
				ai = NO_SCR_AISpawnerComponent.Cast(childTmp.FindComponent(NO_SCR_AISpawnerComponent));
				if(ai) 
				{
						return ai;
				}
				
			}
		}
		return null;
	}
	
	static bool IsPlayer(IEntity ent,ArmaReforgerScripted GameSingleEntity) 
    {
      int playerId = GameSingleEntity.GetPlayerManager().GetPlayerIdFromControlledEntity(ent); 
      return playerId > 0;
    }
	
	static void AddWaypointsToAi(AIAgent agent, IEntity Owner)
	{
		ref array<IEntity> children = new array<IEntity>();
		NO_SCR_DfsStatics.GetAllChildren(Owner,children);
		if(!children || children.Count()<=0) 
		{
			return;
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
				
	}
	
	
}


enum AiGroupType
{
	PleaseSelect = 0,
	FireTeam = 1,
	LightFireTeam = 2,
	MachineGunTeam = 3,
	RifleSquad = 4,
	SentryTeam = 5,
	SniperTeam = 6,
	GLTeam = 7,
	LATTeam = 8,
	SuppressTeam = 9,
	CustomTeam1 = 10,
	CustomTeam2 = 11,
	CustomTeam3 = 12,
	CustomTeam4 = 13,
	CustomTeam5 = 14,
	CustomTeam6 = 15,
	CustomTeam7 = 16,
	CustomTeam8 = 17,
	CustomTeam9 = 18,
	CustomTeam10 = 19,
};