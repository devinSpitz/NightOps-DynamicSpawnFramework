NightOps - DynamicSpawnFramework
=======
![no_dsf](https://user-images.githubusercontent.com/69326203/172024097-b7be9bd0-f399-4eb4-a465-b5e73e4d6b57.png)

Preview Video:

https://stream.spitz.onl/play/38

This framework can be used by everyone and in every mission to spawn AI, Vehicles, Objects, Triggers, and all sorts of prefabs dynamically via trigger or remote execution. Have fun making your missions!

 - It does spawn and despawns on player contact just with hierarchy no naming
 - It does add the waypoint automatically to the ai without naming them as in the example shown and only how in the example shown :D
 - It does recreate the navmesh for all created env if wanted
 - It does only spawn a percentage amount of the ai randomized if wanted
 - It does respawn Ai after dying if wanted
 - It does spawn Vehicles and Environment
 - It can be called outside of the trigger function by script by simple calling Spawn() or Despawn()
 - It does support 5 types of Waypoints: Patrol, Cycle, GetInNearest, Defend, Move
 - It does support dynamic multi spawn by the count of players so you can adapt to how many players are there
  
The Example world is in worlds/CoopTestTerrain.ent  
  
Eniroment Spawn Options  
![image](./Art/172024630-552f385d-93fb-47ff-8a88-0b69dbe799a3.png?raw=true)

Ai Spawn Options  
![image](./Art/172024757-31f2a636-f181-46c3-8432-7a01d6dbd980.png?raw=true)

Ai prefabs  
![image](./Art/172024648-5f37f46e-647d-4326-a623-63e10e5be250.png?raw=true)

Trigger options  
![image](./Art/172024696-587c4b99-aa31-4c3d-9f19-e6493bd4473e.png?raw=true)




How To Preview
===

![image](./Art/FirstStepPreview.png?raw=true)

![image](./Art/SecondStepPreview.png?raw=true)

Spawn by Code example
===

Trigger Options:  
![image](./Art/172024798-924e851e-cb79-47dc-a8ff-78aa4244efbb.png?raw=true)

Code:  
```C#
private RplComponent m_pRplComponent;

if(!m_pRplComponent)
{
 m_pRplComponent = RplComponent.Cast(Owner.FindComponent(RplComponent));
}

if(m_pRplComponent.IsMaster())
{
  NO_SCR_SpawnTrigger.Cast(GetWorld().FindEntityByName("Name of the Trigger")).Spawn();
}
```


Known Issues:
=====
Some default prefabs has disabled RplComponent on them or even no RplComponent. It looks like this:  
![image](./Art/RplDisabled.png?raw=true)

Cause of that you have to do this steps if a prefab only spawns on the master:  
1. Clone it to you project  
![image](./Art/RplFirst.png?raw=true)  
2. Spawn it  
![image](./Art/RplSecond.png?raw=true)  
3. Break prefab Instance until its red  
![image](./Art/BreakPrefab.png?raw=true)  
4. Enable or add all the missing RplComponents  
![image](./Art/RplThird.png?raw=true)  
5. Remove the old prefab  
![image](./Art/RplDel.png?raw=true)  
5. Make a new one with drag and drop  
![image](./Art/DragAndDrop.png?raw=true)  
6. Replace it on the spawner and delete the preview that we spawned and it should work :D  
![image](./Art/Replace.png?raw=true)  


Donate:
=======
Feel free to support my work by donating:  

<a href="https://www.paypal.com/donate?hosted_button_id=JYNFKYARZ7DT4">
<img src="https://www.paypalobjects.com/en_US/CH/i/btn/btn_donateCC_LG.gif" alt="Donate with PayPal" />
</a>   


Credits:
=======
* Sample Coop Addon  
https://github.com/exocs/Reforger-Sample-Coop



Legal
=======
© 2022 BOHEMIA INTERACTIVE a.s. ARMA REFORGER®, ENFUSION®, and BOHEMIA INTERACTIVE® are registered trademarks of BOHEMIA INTERACTIVE a.s. All rights reserved.  
See https://community.bistudio.com/wiki/Category:Arma_Reforger for more information.  
License: https://www.bohemia.net/community/licenses/arma-public-license
