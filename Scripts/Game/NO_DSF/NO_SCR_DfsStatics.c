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
	
	
}


enum AiGroupType
{
	FireTeam,
	LightFireTeam,
	MachineGunTeam,
	RifleSquad,
	SentryTeam,
	SniperTeam,
	GLTeam,
	LATTeam,
	SuppressTeam,
	CustomTeam1,
	CustomTeam2,
	CustomTeam3,
	CustomTeam4,
	CustomTeam5,
	CustomTeam6,
	CustomTeam7,
	CustomTeam8,
	CustomTeam9,
	CustomTeam10,
};