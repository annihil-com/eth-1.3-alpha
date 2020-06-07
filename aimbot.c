// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
All aimbot related funtions
==============================
*/

#include "eth.h"
#include "offsets.h"

void fillHitboxCorrection() {
	hitboxDef[HITBOX_ETPRO][STANDING] = (vec3_union) {-1.0, 0.0, 6.5};
	hitboxDef[HITBOX_ETPRO][CROUCH] = (vec3_union){-1.0, 0.0, 6.5};
	hitboxDef[HITBOX_ETPRO][PRONE] = (vec3_union){1.5, -1.0, 6.5};

	hitboxDef[HITBOX_ETMAIN][STANDING] = (vec3_union){0.0, 0.0, 4.0};
	hitboxDef[HITBOX_ETMAIN][CROUCH] = (vec3_union){0.0, 0.0, 4.0};
	hitboxDef[HITBOX_ETMAIN][PRONE] = (vec3_union){0.0, 0.0, 4.0};

	hitboxDef[HITBOX_ETPUB_REALHEAD][STANDING] = (vec3_union){1.0, -1.0, 6.5};
	hitboxDef[HITBOX_ETPUB_REALHEAD][CROUCH] = (vec3_union){1.0, -1.0, 6.5};
	hitboxDef[HITBOX_ETPUB_REALHEAD][PRONE] = (vec3_union){2.0, 0.0, 6.5};

	hitboxDef[HITBOX_ETPUB_UNFIXED][STANDING] = (vec3_union){0.0, 0.0, 4.0};
	hitboxDef[HITBOX_ETPUB_UNFIXED][CROUCH] = (vec3_union){0.0, 0.0, 4.0};
	hitboxDef[HITBOX_ETPUB_UNFIXED][PRONE] = (vec3_union){0.0, 0.0, 4.0};

	hitboxDef[HITBOX_GENERAL][STANDING] = (vec3_union){0.5, 0.0, 6.5};
	hitboxDef[HITBOX_GENERAL][CROUCH] = (vec3_union){0.5, 0.0, 6.5};
	hitboxDef[HITBOX_GENERAL][PRONE] = (vec3_union){0.5, 0.0, 6.5};
}

void snapshotPredict( int player ){
	//vec3_t a;
	//vec3_t vo;
	vec3_t va;
	//vec3_t dPos;
	float dt;
	int curTime = orig_syscall( CG_MILLISECONDS );
		
	if( (curTime - (int) history[player][0].t)  > 150  || !history[player][1].t || !history[player][2].t ){
		memset( history[player][0].prPos, 0, sizeof(vec3_t));
		return;
	}

	dt = 1000.0 / (history[player][0].t - history[player][1].t);
	va[0] = (history[player][0].trPos[0] - history[player][1].trPos[0])*dt;
	va[1] = (history[player][0].trPos[1] - history[player][1].trPos[1])*dt;
	va[2] = (history[player][0].trPos[2] - history[player][1].trPos[2])*dt;

	// interpolate roughly 2 frames ahead
	history[player][0].prTime = (curTime - history[player][2].t)/333.3;
	VectorCopy(va, history[player][0].prPos );
	
/*		
	dt = 1000.0 / (history[player][0].t - history[player][1].t);
	va[0] = (history[player][0].trPos[0] - history[player][1].trPos[0])*dt;
	va[1] = (history[player][0].trPos[1] - history[player][1].trPos[1])*dt;
	va[2] = (history[player][0].trPos[2] - history[player][1].trPos[2])*dt;
		
	dt = 1000.0 / (history[player][1].t - history[player][2].t);
	vo[0] = (history[player][1].trPos[0] - history[player][2].trPos[0])*dt;
	vo[1] = (history[player][1].trPos[1] - history[player][2].trPos[1])*dt;
	vo[2] = (history[player][1].trPos[2] - history[player][2].trPos[2])*dt;
		
	dt = 1000.0 / (history[player][0].t - history[player][1].t);
	a[0] = (va[0] - vo[0])*dt;
	a[1] = (va[1] - vo[1])*dt;
	a[2] = (va[2] - vo[2])*dt;

	history[player][0].prTime = curTime;

	dt = ((float)curTime - history[player][0].t)/1000.0;
	dPos[0] = va[0]*dt + 0.25*a[0]*dt*dt;
	dPos[1] = va[1]*dt + 0.25*a[1]*dt*dt;
	dPos[2] = va[2]*dt + 0.25*a[2]*dt*dt;

		
	VectorCopy(dPos, history[player][0].prPos );

	if( dPos[0] < -999.9 ||dPos[0] > 999.9 ||
		   dPos[1] < -999.9 ||dPos[1] > 999.9 ||
		   dPos[2] < -999.9 ||dPos[2] > 999.9 ){
		history[player][0].prPos[0] = 0;
		history[player][0].prPos[1] = 0;
		history[player][0].prPos[2] = 0;
		return;
	}*/
}

void aimAt(vec3_t target) {
	#ifdef ETH_DEBUG
		if (seth.value[VAR_AIM_POINT] == AIM_POINT_2D) {
			#define POINT_SIZE 4
	    	int x, y;
			if (worldToScreen(target, &x, &y))
				drawFillRect(x - (POINT_SIZE / 2), y - (POINT_SIZE / 2), POINT_SIZE, POINT_SIZE, colorYellow);
		} else if (seth.value[VAR_AIM_POINT] == AIM_POINT_3D) {
			crossRailTrail(target);
		}
	#endif

	vec3_t org, ang;

	// Self Prediction
	if (seth.value[VAR_SELFPREDICT]) {
		vec3_t pVieworg;
		if (seth.value[VAR_SELFPREDICT]) {
			VectorMA(eth.cg_refdef.vieworg, (float)(eth.cg_frametime) / 1000.0f, eth.cg_entities[eth.cg_clientNum].currentState->pos.trDelta, pVieworg);
		} else {
			ethLog("error: invalid aim prediction type: %i", seth.value[VAR_SELFPREDICT]);
			VectorCopy(eth.cg_refdef.vieworg, pVieworg);
		}
		VectorSubtract(target, pVieworg, org);
    } else {																											
		VectorSubtract(target, eth.cg_refdef.vieworg, org);
    }
	vectoangles(org, ang);

	ang[PITCH] -= eth.cg_refdefViewAngles[PITCH];
	ang[YAW] -= eth.cg_refdefViewAngles[YAW];

	// Simple human aim for now - will try to make it cg_time based, not framebased like now
	if (seth.value[VAR_HUMAN]) {
		while (ang[PITCH] < -360.0f)
			ang[PITCH] += 360.0f;
		while (ang[PITCH] > 360.0f)
			ang[PITCH] -= 360.0f;
		while (ang[YAW] < -360.0f)
			ang[YAW] += 360.0f;
		while (ang[YAW] > 360.0f)
			ang[YAW] -= 360.0f;
		if (ang[PITCH] > 180.0f)
			ang[PITCH] -= 360.0f;
		if (ang[PITCH] < -180.0f)
			ang[PITCH] += 360.0f;
		if (ang[YAW] > 180.0f)
			ang[YAW] -= 360.0f;
		if (ang[YAW] < -180.0f)
			ang[YAW] += 360.0f;
		ang[PITCH] *= seth.value[VAR_HUMANVALUE];
		ang[YAW] *= seth.value[VAR_HUMANVALUE];
	}

    *(float *)sethET->cl_mouseDx += ang[PITCH];
    *(float *)sethET->cl_mouseDy += ang[YAW];
//    *(float *)CL_MOUSEDX_ADDR += AngleDelta(ang[PITCH], seth.refdefViewAngles[PITCH]);
//    *(float *)CL_MOUSEDY_ADDR += AngleDelta(ang[YAW], seth.refdefViewAngles[YAW]);
}

void rifleAimAt(vec3_t target) {
	vec3_t org, ang;

	VectorSubtract(target, eth.cg_refdef.vieworg, org);
	vectoangles(org, ang);


	// Calculate y distance
	vec3_t me, enemy;
	VectorCopy(eth.cg_refdef.vieworg, me);
	VectorCopy(target, enemy);
	me[ROLL] = 0;
	enemy[ROLL] = 0;
	float y = target[ROLL] - eth.cg_refdef.vieworg[ROLL];

	float v = 2000;	// Riffle speed
	float g = DEFAULT_GRAVITY;
	float x = VectorDistance(me, enemy);
	
	float angle = atan((pow(v, 2) - (sqrt(pow(v, 4) + (g * ((g * pow(x, 2)) + (2.0f * y * pow(v, 2))))))) / (g * x));
	angle = RAD2DEG(angle);
	ang[PITCH] = angle;


	ang[PITCH] -= eth.cg_refdefViewAngles[PITCH];
	ang[YAW] -= eth.cg_refdefViewAngles[YAW];

    *(float *)sethET->cl_mouseDx += ang[PITCH];
    *(float *)sethET->cl_mouseDy += ang[YAW];
}

// This tag are in priority order. The first tags are near the center of the
// models, the last tag are at edge of the models
static char *modelBodyTags[] = {
	"tag_chest",
	"tag_torso",
	"tag_bright",
	"tag_lbelt",
	"tag_bleft",
	"tag_ubelt",
	"tag_back",
	"tag_armleft",
	"tag_weapon",
	"tag_armright",
	"tag_weapon2",
	"tag_legright",
	"tag_legleft",
	"tag_footleft",
	"tag_footright"
};

qboolean getVisibleModelBodyTag(const int player, vec3_t *origin) {
	int count = 0;
	for (; count < (sizeof(modelBodyTags) / sizeof(char *)); count++) {
		char *tagText = modelBodyTags[count];
		orientation_t tagOrientation;

		if (!eth_CG_GetTag(player, tagText, &tagOrientation))
			ethLog("error: can't find tag %i[%s] for player: %i", count, tagText, player);

		if (isVisible(tagOrientation.origin, NULL, NULL )) {
			VectorCopy(tagOrientation.origin, *origin);
			#ifdef ETH_DEBUG
				ethDebug("found visible tag: %s for player: %i", tagText, player);
			#endif
			return qtrue;
		}
	}
	return qfalse;
}

void doAutoShoot(qboolean shoot) {
	// We don't want autoshoot
	if (!seth.value[VAR_AUTOSHOOT])
		return;

	// Don't shoot if we're specing/following a player
	if (eth.cg_clientNum != eth.cg_snap->ps.clientNum)
		return;

	qboolean trigger;
// *----------------------------------------------------------------------*
	static int anylastShootTime = 0;
	if ((eth.cg_time - anylastShootTime) >= (seth.value[VAR_SNIPERDELAY])) {
		anylastShootTime = eth.cg_time;
		setAction(ACTION_ATTACK, 0); // u can remove it for testing so leave return only;
		return;
	}
// *----------------------------------------------------------------------*
	
	if (seth.value[VAR_AUTOSHOOT_TYPE] == AUTOSHOOT_TYPE_TRIGGER) {
		// Set a far away end point
		vec3_t aheadPos;
		VectorMA(eth.cg_refdef.vieworg, 8192, eth.cg_refdef.viewaxis[0], aheadPos);
	
		// Get trace for this point
		trace_t trace;
		eth.CG_Trace(&trace, eth.cg_refdef.vieworg, NULL, NULL, aheadPos, eth.cg_snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_ITEM);
		trigger = IS_PLAYER_ENEMY(trace.entityNum);
	// Don't use trigger
	} else {
		trigger = qtrue;
	}

	// For tce
	if (eth.mod->type == MOD_TCE) {
		if (shoot && trigger && (syscall_CG_Key_IsDown(K_MOUSE1) || (seth.value[VAR_AUTOSHOOT] == AUTOSHOOT_ON))) {
			static qboolean lastFrameAttack = qfalse;
			if (!lastFrameAttack) {
				setAction(ACTION_ATTACK, 1);
				lastFrameAttack = qtrue;
			} else {
				setAction(ACTION_ATTACK, 0);
				lastFrameAttack = qfalse;
			}
		} else {
			setAction(ACTION_ATTACK, 0);
		}
	// For all others mod
	} else {
		if (shoot && trigger && (syscall_CG_Key_IsDown(K_MOUSE1) || (seth.value[VAR_AUTOSHOOT] == AUTOSHOOT_ON))) {
			// Check for sniper weapon delay
			if (((eth.cg_entities[eth.cg_clientNum].currentState->weapon == WP_K43_SCOPE)
						|| (eth.cg_entities[eth.cg_clientNum].currentState->weapon == WP_GARAND_SCOPE)) && !BG_PlayerMounted(eth.cg_snap->ps.eFlags)) {
				static int lastShootTime = 0;
				if ((eth.cg_time - lastShootTime) >= (seth.value[VAR_SNIPERDELAY])) {
					setAction(ACTION_ATTACK, 1);
					lastShootTime = eth.cg_time;
				} else {
					setAction(ACTION_ATTACK, 0);
				}
			// Check for weapon overheating
			#define OVERHEATING_LIMIT 200
			} else if (seth.value[VAR_OVERHEAT] && (eth.cg_snap->ps.curWeapHeat > OVERHEATING_LIMIT)) {
				setAction(ACTION_ATTACK, 0);
			} else {
				setAction(ACTION_ATTACK, 1);
				#ifdef ETH_DEBUG
					ethDebug("autoshoot: shoot on");
				#endif
			}
		} else {
			setAction(ACTION_ATTACK, 0);
			#ifdef ETH_DEBUG
				ethDebug("autoshoot: shoot off");
			#endif
		}
	}
}

void doSatchelAutoShoot() {
	int satchelEntityNum = findSatchel();

	// If don't find satchel
	if (satchelEntityNum == -1) {
		#ifdef ETH_DEBUG
			ethDebug("autoshoot: don't find satchel for %i", eth.cg_snap->ps.clientNum);
		#endif
		setAction(ACTION_ATTACK, 0);
		return;
	}

	// Find an enemy near the satchel
	#define SATCHEL_SHOOT_DISTANCE 100
	int entityNum = 0;
	for (; entityNum < MAX_CLIENTS; entityNum++) {
		ethEntity_t *entity = &eth.entities[entityNum];
		
		if (entity->isValid
				&& IS_PLAYER_ENEMY(entityNum)
				&& (VectorDistance(entity->origin, eth.entities[satchelEntityNum].origin) < SATCHEL_SHOOT_DISTANCE)) {
			setAction(ACTION_ATTACK, 1);
			return;
		}
	}

	// TODO: find a breakable object near the satchel

	setAction(ACTION_ATTACK, 0);
}


// Jaymod 2.1.x changes quite a lot of things, more attention needed
int findTarget(int targetFlag) {
	int nearest = -1;
	int maxEntities = MAX_GENTITIES;
	
	// Get max entities
	if (targetFlag & TARGET_PLAYER)
		maxEntities = MAX_CLIENTS;

	int entityNum = 0;

	if( (seth.value[VAR_TARGET_PRIO] == TARGET_ATTACKER ) ){
		int ent = eth.cg_snap->ps.persistant[PERS_ATTACKER];
		if( ent >= 0 && ent <= MAX_CLIENTS ){
			// Jaymod 2.1.x aimbot workaround
			if( eth.mod->crc32 == 0x5f24fa23 || eth.mod->crc32 == 0x8352a951 )
				eth.cg_entities[ent].currentState->eType = ET_PLAYER;

			if( ((targetFlag & TARGET_ENEMY) && (targetFlag & TARGET_PLAYER) && IS_PLAYER_ENEMY(entityNum))
				&& eth.entities[ent].isVisible && !IS_PLAYER_DEAD(ent) )
				return ent;
		}
	}
	
	for (; entityNum < maxEntities; entityNum++) {
		ethEntity_t* entity = &eth.entities[entityNum];

		if (!*eth.cg_entities[entityNum].currentValid || (entityNum == eth.cg_snap->ps.clientNum))
			continue;

		// Jaymod 2.1.x aimbot workaround
		if( entityNum < MAX_CLIENTS && (eth.mod->crc32 == 0x5f24fa23 || eth.mod->crc32 == 0x8352a951) )
			eth.cg_entities[entityNum].currentState->eType = ET_PLAYER;
		
		// Player
		if ((targetFlag & TARGET_PLAYER) && !IS_PLAYER_VALID(entityNum))
			continue;
		if ((targetFlag & TARGET_PLAYER_VULN) && !IS_PLAYER_VULNERABLE(entityNum))
			continue;
		if ((targetFlag & TARGET_PLAYER_ALIVE) && IS_PLAYER_DEAD(entityNum))
			continue;
		if ((targetFlag & TARGET_PLAYER_DEAD) && !IS_PLAYER_DEAD(entityNum))
			continue;

		// Target
		if ((targetFlag & TARGET_PLAYER_HEAD) && !entity->isHeadVisible)
			continue;
		if ((targetFlag & TARGET_PLAYER_BODY) && VectorCompare(entity->bodyPart, vec3_origin))
			continue;
			
		// Targeting specific player has priority over other settings
		if ( (targetFlag & TARGET_VISIBLE) && entity->isVisible &&
			(seth.value[VAR_TARGET_PRIO]==TARGET_PERSON) && eth.clAttack[entityNum] )
			return entityNum;

		// Common
		if ((targetFlag & TARGET_ENEMY) && (targetFlag & TARGET_PLAYER) && !IS_PLAYER_ENEMY(entityNum))
			continue;
		if ((targetFlag & TARGET_ENEMY) && (targetFlag & TARGET_MISSILE) && !IS_MISSILE_ENEMY(entityNum))
			continue;
		if ((targetFlag & TARGET_FRIEND) && (targetFlag & TARGET_PLAYER) && IS_PLAYER_ENEMY(entityNum))
			continue;	
		if ((targetFlag & TARGET_FRIEND) && (targetFlag & TARGET_MISSILE) && IS_MISSILE_ENEMY(entityNum))
			continue;	
		if ((targetFlag & TARGET_VISIBLE) && !entity->isVisible)
			continue;
		if ((targetFlag & TARGET_NOTVISIBLE) && entity->isVisible)
			continue;

		// Missile
		if ((targetFlag & TARGET_MISSILE) && !IS_MISSILE(entityNum))
			continue;
		if ((targetFlag & TARGET_MISSILE_ARMED) && !IS_MISSILE_ARMED(entityNum))
			continue;
		if ((targetFlag & TARGET_MISSILE_NOTARMED) && IS_MISSILE_ARMED(entityNum))
			continue;
		if ( (targetFlag & TARGET_MISSILE_DYNAMITE) && !IS_DYNAMITE(entityNum))
			continue;
			
		if( (seth.value[VAR_TARGET_PRIO] == TARGET_NOOBTUBE) && IS_PANZER(entityNum) && !IS_PLAYER_ENEMY(entityNum) )
			return entityNum;
		if( (seth.value[VAR_TARGET_PRIO] == TARGET_MG42) && IS_MG42(entityNum) && !IS_PLAYER_ENEMY(entityNum) )
			return entityNum;
		if( (seth.value[VAR_TARGET_PRIO] == TARGET_HEAVY) && (IS_HEAVYWP(entityNum) && !IS_PLAYER_ENEMY(entityNum)))
			return entityNum;		

		if (seth.value[VAR_AIMFOV] != 360.00f) {
			vec3_t aimPoint, ang, org;
			if (eth.entities[entityNum].isHeadVisible)
				VectorCopy(eth.entities[entityNum].head, aimPoint);
			else if (seth.value[VAR_AIM_PRIO] != AIM_PRIO_HEAD_ONLY)
				VectorCopy(eth.entities[entityNum].bodyPart, aimPoint);
			else {
				ethLog("findTarget: can't find a point to aim");
				return -1;
			}

			VectorSubtract(aimPoint, eth.cg_refdef.vieworg, org);
			vectoangles(org, ang);

			ang[PITCH] -= eth.cg_refdefViewAngles[PITCH];
			ang[YAW] -= eth.cg_refdefViewAngles[YAW];


			if (ang[PITCH] < 0.0)
				ang[PITCH] *= -1.0;
			if (ang[YAW] < 0.0)
				ang[YAW] *= -1.0;

			if (ang[PITCH] > 360.0f)
				ang[PITCH] -= 360.0f;
			if (ang[YAW] > 360.0f)
				ang[YAW] -= 360.0f;

			float aimFov[2];

			aimFov[0] = (ang[PITCH] - 360.0) * -1.0;
			aimFov[1] = (ang[YAW] - 360.0) * -1.0;

			if (ang[PITCH] > aimFov[0])
				ang[PITCH] = aimFov[0];
			if (ang[YAW] > aimFov[1])
				ang[YAW] = aimFov[1];

			if ((ang[PITCH] > seth.value[VAR_AIMFOV]) || (ang[YAW] > seth.value[VAR_AIMFOV]))
				continue;
		}

		if ((entity->distance != 0)
				&& (nearest == -1 || (entity->distance < eth.entities[nearest].distance))) {
			nearest = entityNum;
		}
	}
	#ifdef ETH_DEBUG
		if (nearest != -1)
			ethDebug("findTarget: target found for flags 0x%x", targetFlag);
		else
			ethDebug("findTarget: target not found for flags 0x%x", targetFlag);
	#endif
	return nearest;
}

void blockMouse(qboolean state) {
	if (!seth.value[VAR_BLOCK_MOUSE])
		return;

	#define SENSITIVITY_STR "sensitivity"
	static float sensitivity = 0;	// For backup user sensitivity
	static qboolean mouseBlocked = qfalse;	// Current state
	char buffer[32];

	// Set sensitivity
	if (state && !mouseBlocked) {
		syscall_CG_Cvar_VariableStringBuffer(SENSITIVITY_STR, buffer, sizeof(buffer));
		sensitivity = atof(buffer);

		syscall_CG_SendConsoleCommand(SENSITIVITY_STR " 0\n");
		mouseBlocked = qtrue;
		#ifdef ETH_DEBUG
			ethDebug("blockMouse: mouse off");
		#endif
	} else if (!state && mouseBlocked) {
		snprintf(buffer, sizeof(buffer), SENSITIVITY_STR " %f\n", sensitivity);
		syscall_CG_SendConsoleCommand(buffer);
		mouseBlocked = qfalse;
		#ifdef ETH_DEBUG
			ethDebug("blockMouse: mouse on");
		#endif
	}
					
}

void doAimbot() {
	qboolean autoShootWeapon = qfalse;
	int targetFlag[2] = { -1, -1 };	// If a targetFlag failed try the next one

	// For tce
	if (eth.mod->type == MOD_TCE) {

    	int clips = eth.cg_snap->ps.ammoclip[eth.weapon];
		int ammo = eth.cg_snap->ps.ammo[eth.weapon];
		static qboolean lastframeReload = qfalse;
	
		if(clips == 0 && ammo > 0) {
		    if(eth.weapon != WP_KNIFE)
			if(!lastframeReload) {
			    setAction(ACTION_RELOAD, 1);
			    lastframeReload = qtrue;
			    return;
			}
		}
		if(lastframeReload) {
			setAction(ACTION_RELOAD, 0);
			lastframeReload = qfalse;
		}
			
		autoShootWeapon = qtrue;
		targetFlag[0] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_ALIVE;

		// Aim priority
		if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD_BODY) {
			targetFlag[0] |= TARGET_PLAYER_HEAD_BODY;
		} else if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD_ONLY) {
			targetFlag[0] |= TARGET_PLAYER_HEAD;
		} else if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD) {
			targetFlag[1] = targetFlag[0] | TARGET_PLAYER_BODY;
			targetFlag[0] |= TARGET_PLAYER_HEAD;
		}
	// For all others mod
	} else {
		// if player is mounted (mobile mg42, static mg42) aim for body
		if (BG_PlayerMounted(eth.cg_snap->ps.eFlags) || (eth.weapon == WP_MOBILE_MG42) || (eth.weapon == WP_MOBILE_MG42_SET)) {
			autoShootWeapon = qtrue;
			targetFlag[0] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_HEAD;

			if (!seth.value[VAR_AIM_DEAD])
				targetFlag[0] |= TARGET_PLAYER_ALIVE;
			
			targetFlag[1] = targetFlag[0] | TARGET_PLAYER_BODY;
		} else {
		// different targets for differents weapons
			switch (eth.weapon) {
				case WP_LUGER:
				case WP_SILENCER:
				case WP_AKIMBO_LUGER:
				case WP_AKIMBO_SILENCEDLUGER:
				case WP_COLT:
				case WP_SILENCED_COLT:
				case WP_AKIMBO_COLT:
				case WP_AKIMBO_SILENCEDCOLT:
				case WP_MP40:
				case WP_THOMPSON:
				case WP_GARAND:
				case WP_K43:
				case WP_FG42:
				case WP_STEN:
				case WP_CARBINE:
				case WP_KAR98:
					autoShootWeapon = qtrue;
					targetFlag[0] = TARGET_PLAYER_ENEMY;
		
					// Aim at dead player
					if (!seth.value[VAR_AIM_DEAD])
						targetFlag[0] |= TARGET_PLAYER_ALIVE;
		
					// Aim priority
					if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD_BODY) {
						targetFlag[0] |= TARGET_PLAYER_HEAD_BODY;
					} else if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD_ONLY) {
						targetFlag[0] |= TARGET_PLAYER_HEAD;
					} else if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_HEAD) {
						targetFlag[1] = targetFlag[0] | TARGET_PLAYER_BODY;
						targetFlag[0] |= TARGET_PLAYER_HEAD;
					} else if (seth.value[VAR_AIM_PRIO] == AIM_PRIO_BODY_ONLY) {
						targetFlag[0] |= TARGET_PLAYER_BODY;
					}
					break;
				case WP_GPG40:
				case WP_M7:
					autoShootWeapon = qtrue;
					targetFlag[0] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_ALIVE | TARGET_PLAYER_BODY;
					break;
				case WP_GARAND_SCOPE:
				case WP_K43_SCOPE:
				case WP_FG42SCOPE:
					autoShootWeapon = qtrue;
					targetFlag[0] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_ALIVE | TARGET_PLAYER_HEAD;
					break;
				case WP_KNIFE:
					targetFlag[0] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_ALIVE | TARGET_PLAYER_HEAD;
					targetFlag[1] = TARGET_PLAYER_ENEMY | TARGET_PLAYER_DEAD | TARGET_PLAYER_HEAD;
					break;
				case WP_MEDIC_SYRINGE:
					targetFlag[0] = TARGET_PLAYER | TARGET_FRIEND | TARGET_PLAYER_DEAD | TARGET_PLAYER_BODY;
					break;
				case WP_PLIERS:
					targetFlag[0] = TARGET_MISSILE | TARGET_MISSILE_DYNAMITE | TARGET_ENEMY | TARGET_MISSILE_ARMED;
					targetFlag[1] = TARGET_MISSILE | TARGET_MISSILE_DYNAMITE | TARGET_FRIEND | TARGET_MISSILE_NOTARMED;
				default:
					break;
			}
		}
	}

	// Check if we want aimbot
	if ((syscall_CG_Key_IsDown(K_MOUSE1) || seth.value[VAR_AIM_AUTO])
			&& (targetFlag[0] != -1) && seth.value[VAR_AIM_PRIO] &&
			(eth.cg_snap->ps.clientNum == eth.cg_clientNum) ) {
		// Find target aim point
		int count = 0;
		int target;
		for (; count < (sizeof(targetFlag) / sizeof(int)); count++) {
			if (targetFlag[count] != -1) {
				vec3_t aimPoint;
				VectorCopy(vec3_origin, aimPoint);

				target = findTarget(targetFlag[count]);

#ifdef ETH_DEBUG
				ethDebug("target: found target %i %s targetFlag[0]=%i targetFlag[1]=%i", target, eth.clientInfo[target].name, targetFlag[0], targetFlag[1]);
#endif
				
				// No target found
				if (target == -1)
					continue;

				if( seth.value[VAR_ENTCAM] == ENTCAM_TARGET )
					eth.entCamTarget = target;
				
				doPrediction(target);
				doVecZ(target);
								
				// Target found
				if (targetFlag[count] & TARGET_PLAYER) {
					if (targetFlag[count] & TARGET_PLAYER_HEAD) {
						VectorCopy(eth.entities[target].head, aimPoint);
						#ifdef ETH_DEBUG
							ethDebug("aimbot: found head player: %s", eth.clientInfo[target].name);
						#endif
					} else if (targetFlag[count] & TARGET_PLAYER_BODY) {
						VectorCopy(eth.entities[target].bodyPart, aimPoint);
						#ifdef ETH_DEBUG
							ethDebug("aimbot: found body player: %s", eth.clientInfo[target].name);
						#endif
					} else if (targetFlag[count] & TARGET_PLAYER_HEAD_BODY) {
						// Head or body ?
						if (eth.entities[target].isHeadVisible)
							VectorCopy(eth.entities[target].head, aimPoint);
						else
							VectorCopy(eth.entities[target].bodyPart, aimPoint);
			
						#ifdef ETH_DEBUG
							ethDebug("aimbot: found head/body player: %s", eth.clientInfo[target].name);
						#endif
					} else {
						ethLog("error: target found but no aimpoint set in target flags");
					}
				} else if (targetFlag[count] & TARGET_MISSILE) {
					VectorCopy(eth.entities[target].origin, aimPoint);
					#ifdef ETH_DEBUG
						ethDebug("aimbot: found missile: %i", target);
					#endif
				} else {
					ethLog("error: target found but it's not a player/missile");
				}
				
				// If aim point is set, aim and shoot
				if (!VectorCompare(aimPoint, vec3_origin)) {
					if (!seth.value[VAR_HUMAN])
					    doAutoShoot(qtrue);

					if ((eth.weapon == WP_GPG40) || (eth.weapon == WP_M7))
						rifleAimAt(aimPoint);
					else
						aimAt(aimPoint);

				    blockMouse(qtrue);
					return;
				}
			}
		}
		#ifdef ETH_DEBUG
			ethDebug("aimbot: no target found");
		#endif
	}

	blockMouse(qfalse);

	// For satchel
	if (seth.value[VAR_SATCHEL_AUTOSHOOT]
			&& (eth.cg_entities[eth.cg_clientNum].currentState->weapon == WP_SATCHEL_DET)
			&& (eth.mod->type != MOD_TCE)) {
		doSatchelAutoShoot();
	// All others weapons
	} else {
		doAutoShoot(!autoShootWeapon);
	}
}

void doVecZ(int player) {
	ethEntity_t *entity = &eth.entities[player];
	
	// No need if not an enemy
	if (!IS_PLAYER_ENEMY(player))
		return;

	// For tce
	if (eth.mod->type == MOD_TCE) {
		VectorMA(entity->head, seth.value[VAR_VECZ], entity->headAxis[2], entity->head);
	// For all others mod
	} else {
		// Set different vecz for different weapon
		switch (eth.weapon) {
			case WP_LUGER:
			case WP_SILENCER:
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_SILENCEDLUGER:
			case WP_COLT:
			case WP_SILENCED_COLT:
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_SILENCEDCOLT:
			case WP_MP40:
			case WP_THOMPSON:
	 		case WP_MOBILE_MG42:
			case WP_GARAND:
			case WP_K43:
			case WP_FG42:
			case WP_STEN:
			case WP_CARBINE:
			case WP_KAR98:
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
			case WP_FG42SCOPE:
				if (seth.value[VAR_AIMVECZ_TYPE] == VECZ_HITBOX) {
					vec3_t rmin;
					vec3_t rmax;
					VectorCopy(entity->rmin,rmin);
					VectorCopy(entity->rmax,rmax);

					//entity->head[0]=entity->head[0]+(headModel.mins[0]+headModel.maxs[0])*0.5f;
					//entity->head[1]=entity->head[1]+(headModel.mins[1]+headModel.maxs[1])*0.5f;
					//entity->head[2]=entity->head[2]+(headModel.mins[2]+headModel.maxs[2])*0.5f;

					int corrType = (int) seth.value[VAR_HITBOX];
					if( corrType != HITBOX_OFF ){
						//orientation_t or;
						//orig_syscall(CG_R_LERPTAG, &or, &eth.cg_entities[player].pe_bodyRefEnt, "tag_head", 0);
						//AxisToAngles(or.axis, angles);
						// crouching 
						if( eth.cg_entities[player].currentState->eFlags & EF_CROUCHING ){
							entity->head[0] += hitboxDef[corrType][CROUCH].x;
							entity->head[1] += hitboxDef[corrType][CROUCH].y;
							entity->head[2] += hitboxDef[corrType][CROUCH].z;
						// prone bastards
						} else if ( eth.cg_entities[player].currentState->eFlags & EF_PRONE ||
							eth.cg_entities[player].currentState->eFlags & EF_PRONE_MOVING){
							entity->head[0] += hitboxDef[corrType][PRONE].x;
							entity->head[1] += hitboxDef[corrType][PRONE].y;
							entity->head[2] += hitboxDef[corrType][PRONE].z;
							
						} else {
							entity->head[0] += hitboxDef[corrType][STANDING].x;
							entity->head[1] += hitboxDef[corrType][STANDING].y;
							entity->head[2] += hitboxDef[corrType][STANDING].z;								
						}
					}

#ifdef ETH_DEBUG
//Debugstuph - use functions from: http://www.nixcoders.org/forum/index.php?showtopic=538
//					if (seth.value[VAR_HITBOX] && syscall_CG_Key_IsDown(K_MOUSE1)) {
//						drawHeadBox(rmin,rmax,entity->head);
//					}
#endif

				} else if (seth.value[VAR_AIMVECZ_TYPE] == VECZ_MANUAL) {				
				// Simple vecz correction
					switch (eth.weapon) {
						case WP_STEN:
							VectorMA(entity->head, seth.value[VAR_VECZ_STEN], entity->headAxis[2], entity->head);
							break ;
						case WP_K43_SCOPE:
						case WP_GARAND_SCOPE:
						case WP_FG42SCOPE:
							VectorMA(entity->head, seth.value[VAR_VECZ_SCOPE], entity->headAxis[2], entity->head);
							break;
						default:
							VectorMA(entity->head, seth.value[VAR_VECZ], entity->headAxis[2], entity->head);
							break;
					} // Auto vecz
				} else if (seth.value[VAR_AIMVECZ_TYPE] == VECZ_AUTO) {
					#define NEAR_TARGET_VECZ 5.0f

					// Scoped weapons always aim at the middle of head
					if (eth.weapon == WP_GARAND_SCOPE || eth.weapon == WP_K43_SCOPE || eth.weapon == WP_FG42SCOPE) {
						VectorMA(entity->head, 5.0f, entity->headAxis[2], entity->head);
						return;
					}
			
					// Different 'near distance' for sten
					int nearTarget;
					if (eth.weapon == WP_STEN)
						nearTarget = 700.0f;
					else
						nearTarget = 500.0f;

					if (entity->distance <= nearTarget)
						VectorMA(entity->head, NEAR_TARGET_VECZ, entity->headAxis[2], entity->head);
					else if ((entity->distance > nearTarget) && (entity->distance < 2000.0f))
						VectorMA(entity->head, (NEAR_TARGET_VECZ * (100.0f - ((entity->distance - nearTarget) / 15.0f))) / 100.0f, entity->headAxis[2], entity->head);
				}
				break;
			default:
				break;
		}
	}
}


/* 	pos.trDelta is poor predict0r for velocity
	it doesn't include terrain curvature */
void doPrediction(int player) {
	// No predicton
	if (!seth.value[VAR_AIMPREDICT])
		return;
	
	ethEntity_t *entity = &eth.entities[player];
	
	//eth.BG_EvaluateTrajectory(&eth.cg_entities[player].currentState->pos, *eth.cg.time + eth.cg_frametime, entity->origin, qfalse, 0);
	//eth.BG_EvaluateTrajectory(&eth.cg_entities[player].currentState->pos, *eth.cg.time + (seth.value[VAR_AIMPREDICT_VALUE] * 100.0f), entity->origin, qfalse, 0);
	
	// Frametime prediction based
	if (seth.value[VAR_AIMPREDICT] == AIMPREDICT_AUTO_1) {
		VectorMA(entity->origin, (float)(eth.cg_frametime) / 1000.0f, eth.cg_entities[player].currentState->pos.trDelta, entity->origin);
		//VectorMA(entity->head, (float)(eth.cg_frametime) / 1000.0f, eth.cg_entities[player].currentState->pos.trDelta, entity->head);
		VectorMA (entity->head, eth.cg_snap->ping / 800, eth.cg_entities[player].currentState->pos.trDelta, entity->head);
		
	} else if (seth.value[VAR_AIMPREDICT] == AIMPREDICT_AUTO_2) {
		vec3_t velocity;
		VectorScale(eth.cg_entities[player].currentState->pos.trDelta, 1000.0f / ((float)(eth.cg_frametime) - eth.cg_entities[player].currentState->pos.trTime) , velocity);
		VectorMA(entity->origin, (float)(eth.cg_frametime) / 1000.0f, velocity, entity->origin);
		VectorMA(entity->head, (float)(eth.cg_frametime) / 1000.0f, velocity, entity->head);
	} else if (seth.value[VAR_AIMPREDICT] == AIMPREDICT_AUTO_3) {
	    VectorMA(entity->origin,  (eth.myping/2) * 0.001f, eth.cg_entities[player].currentState->pos.trDelta, entity->origin);
		VectorMA(entity->head, (eth.myping/2)  * 0.001f, eth.cg_entities[player].currentState->pos.trDelta, entity->head);
	} else if (seth.value[VAR_AIMPREDICT] == AIMPREDICT_SNAPSHOT ) {
		snapshotPredict(player);
		VectorMA(entity->origin,  (eth.myping/2) * 0.001f, history[player][0].prPos, entity->origin);
		VectorMA(entity->head, (eth.myping/2)  * 0.001f, history[player][0].prPos, entity->head);
	} else
		ethLog("error: invalid aim prediction type: %i", seth.value[VAR_AIMPREDICT]);
}

#ifdef ETH_DEBUG

void doPredictionStats() {
	static int lastTarget = -1;
	int target = -1;

	// Find the nearest moving player
	int count;
	int targetDistance = -1;
	static vec3_t lastPosition[MAX_CLIENTS];
	for (count = 0; count < MAX_CLIENTS; count++) {
		// If valid and moving
		if (IS_PLAYER_VALID(count) && !VectorCompare(lastPosition[count], eth.entities[count].origin)) {
			// If nearest
			if ((targetDistance == -1) || (eth.entities[count].distance < targetDistance)) {
				target = count;
				targetDistance = eth.entities[count].distance;
			}
		}
	}

	// Backup for next frame
	lastTarget = target;

	// Store all players positions for next frame
	for (count = 0; count < MAX_CLIENTS; count++) {
		if (IS_PLAYER_VALID(count))
			VectorCopy(eth.entities[count].origin, lastPosition[count]);
	}

	// If no target found or not the same target
	if ((target == -1) || (lastTarget != target))
		return;

	// Init prediction stats
	ethEntity_t *entity = &eth.entities[target];
	static vec3_t lastOrigin;
	static vec3_t lastPredictOrigin;

	// Draw stats
	drawPredictStats(lastOrigin, entity->origin, lastPredictOrigin);

	printf("---------\n");
	printf("origin : %.1f %.1f %.1f\n", lastOrigin[0], lastOrigin[1], lastOrigin[2]);
	printf("real   : %.1f %.1f %.1f\n", entity->origin[0], entity->origin[1], entity->origin[2]);
	printf("predict: %.1f %.1f %.1f\n", lastPredictOrigin[0], lastPredictOrigin[1], lastPredictOrigin[2]);

	// Backup var for next frame
	VectorCopy(entity->origin, lastOrigin);

	doPrediction(target);

	// Backup var for next frame
	VectorCopy(entity->origin, lastPredictOrigin);
}

#endif // ETH_DEBUG
