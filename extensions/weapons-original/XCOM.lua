------------------------------------------------------------------------------
-- Original X-com Weapons Set (by DiversanT)
------------------------------------------------------------------------------

AddXcomItem {
    cost = 10,
    name = "ELECTRO FLARE",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 55),
    pMap = pck_image("$(xcom)/units/floorob.pck", 72),
    health = 999,
    importance = 1,
    width = 1,
    height = 1,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
    weight = 2,
    minimapMark = 1,
    ownLight = 10,
}



AddXcomItem {
	cost = 0,
	name = "PISTOL",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 3),
	pMap = pck_image("$(xcom)/units/floorob.pck", 3),
	health = 999,
	importance = 5,
	width = 1,
	height = 2,
	ammo = {"PISTOL CLIP"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 96, 8),
	accuracy = {0, 60, 78},
	time = {0, 18, 30},
	weight = 5,
	isGun = 1,
	minimapMark = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 4) or cat_sample("$(xcom)/sound/sample2.cat", 4),
}

AddXcomItem {
	cost = 0,
	name = "PISTOL CLIP",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 4),
	pMap = pck_image("$(xcom)/units/floorob.pck", 4),
	health = 999,
	damage = 26,
	dDeviation = 100,
	importance = 3,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 0,
	rounds = 12,
	disappear = 1,
	weight = 3,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 22) or cat_sample("$(xcom)/sound/sample2.cat", 22),
}

AddXcomItem {
	cost = 50,
	name = "RIFLE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 1),
	pMap = pck_image("$(xcom)/units/floorob.pck", 1),
	health = 999,
	importance = 6,
	width = 1,
	height = 3,
	ammo = {"RIFLE CLIP"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 0, 8),
	accuracy = {35, 60, 110},
	time = {12, 25, 80},
	autoShots = 3,
	weight = 8,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 4) or cat_sample("$(xcom)/sound/sample2.cat", 4),
}

AddXcomItem {
	cost = 20,
	name = "RIFLE CLIP",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 2),
	pMap = pck_image("$(xcom)/units/floorob.pck", 2),
	health = 999,
	damage = 30,
	dDeviation = 100,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 0,
	rounds = 20,
	disappear = 1,
	weight = 3,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 22) or cat_sample("$(xcom)/sound/sample2.cat", 22),
}

AddXcomItem {
	cost = 80,
	name = "HEAVY CANNON",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 11),
	pMap = pck_image("$(xcom)/units/floorob.pck", 11),
	health = 999,
	importance = 14,
	width = 2,
	height = 3,
	ammo = {"CANNON AP-AMMO", "CANNON HE-AMMO", "CANNON I-AMMO"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 24, 8),
	accuracy = {0, 60, 90},
	time = {0, 33, 80},
	weight = 18,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 2) or cat_sample("$(xcom)/sound/sample2.cat", 2),
}

AddXcomItem {
	cost = 40,
	name = "CANNON AP-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 12),
	pMap = pck_image("$(xcom)/units/floorob.pck", 8),
	health = 999,
	damage = 56,
	dDeviation = 100,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 0,
	rounds = 6,
	disappear = 1,
	weight = 6,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 22) or cat_sample("$(xcom)/sound/sample2.cat", 22),
}

AddXcomItem {
	cost = 70,
	name = "CANNON HE-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 13),
	pMap = pck_image("$(xcom)/units/floorob.pck", 9),
	health = 999,
	damage = 52,
	dDeviation = 50,
	exploRange = 3,
	smokeRange = 3,
	smokeTime = 2,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	rounds = 6,
	disappear = 1,
	weight = 6,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 80,
	name = "CANNON I-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 14),
	pMap = pck_image("$(xcom)/units/floorob.pck", 10),
	health = 999,
	damage = 60,
	dDeviation = 50,
	exploRange = 3,
	smokeRange = 3,
	smokeTime = 2,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 1,
	rounds = 6,
	disappear = 1,
	weight = 6,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 100,
	name = "AUTO-CANNON",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 7),
	pMap = pck_image("$(xcom)/units/floorob.pck", 7),
	health = 999,
	importance = 16,
	width = 2,
	height = 3,
	ammo = {"AUTO-CANNON AP-AMMO", "AUTO-CANNON HE-AMMO", "AUTO-CANNON I-AMMO"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 32, 8),
	accuracy = {32, 56, 82},
	time = {14, 33, 80},
	autoShots = 3,
	weight = 19,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 2) or cat_sample("$(xcom)/sound/sample2.cat", 2),
}

AddXcomItem {
	cost = 50,
	name = "AUTO-CANNON AP-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 8),
	pMap = pck_image("$(xcom)/units/floorob.pck", 12),
	health = 999,
	damage = 42,
	dDeviation = 100,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 0,
	rounds = 14,
	disappear = 1,
	weight = 5,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 22) or cat_sample("$(xcom)/sound/sample2.cat", 22),
}

AddXcomItem {
	cost = 90,
	name = "AUTO-CANNON HE-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 9),
	pMap = pck_image("$(xcom)/units/floorob.pck", 13),
	health = 999,
	damage = 44,
	dDeviation = 50,
	exploRange = 3,
	smokeRange = 3,
	smokeTime = 2,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	rounds = 14,
	disappear = 1,
	weight = 5,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 100,
	name = "AUTO-CANNON I-AMMO",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 10),
	pMap = pck_image("$(xcom)/units/floorob.pck", 14),
	health = 999,
	damage = 48,
	dDeviation = 50,
	exploRange = 3,
	smokeRange = 3,
	smokeTime = 2,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 1,
	rounds = 14,
	disappear = 1,
	weight = 5,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 100,
	name = "ROCKET LAUNCHER",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 15),
	pMap = pck_image("$(xcom)/units/floorob.pck", 15),
	health = 999,
	importance = 18,
	width = 2,
	height = 3,
	ammo = {"SMALL ROCKET", "LARGE ROCKET", "INCENDIARY ROCKET"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 72, 8),
	accuracy = {0, 55, 115},
	time = {0, 45, 75},
	weight = 10,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 52) or cat_sample("$(xcom)/sound/sample2.cat", 52),
}

AddXcomItem {
	cost = 200,
	name = "SMALL ROCKET",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 16),
	pMap = pck_image("$(xcom)/units/floorob.pck", 16),
	health = 999,
	damage = 75,
	dDeviation = 50,
	exploRange = 4,
	smokeRange = 4,
	smokeTime = 2,
	importance = 5,
	width = 1,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 80, 8),
	damageType = 2,
	rounds = 1,
	disappear = 1,
	weight = 6,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 300,
	name = "LARGE ROCKET",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 17),
	pMap = pck_image("$(xcom)/units/floorob.pck", 17),
	health = 999,
	damage = 100,
	dDeviation = 50,
	exploRange = 6,
	smokeRange = 6,
	smokeTime = 2,
	importance = 6,
	width = 1,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 80, 8),
	damageType = 2,
	rounds = 1,
	disappear = 1,
	weight = 8,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 250,
	name = "INCENDIARY ROCKET",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 18),
	pMap = pck_image("$(xcom)/units/floorob.pck", 18),
	health = 999,
	damage = 90,
	dDeviation = 50,
	exploRange = 5,
	smokeRange = 5,
	smokeTime = 2,
	importance = 7,
	width = 1,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 80, 8),
	damageType = 1,
	rounds = 1,
	disappear = 1,
	weight = 8,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound2.cat", 10) or cat_sample("$(xcom)/sound/sample.cat", 10),
}

AddXcomItem {
	cost = 150,
	name = "LASER PISTOL",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 5),
	pMap = pck_image("$(xcom)/units/floorob.pck", 5),
	health = 999,
	damage = 46,
	dDeviation = 100,
	importance = 4,
	width = 1,
	height = 2,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 112, 8),
	damageType = 3,
	accuracy = {28, 40, 68},
	time = {8, 20, 55},
	autoShots = 3,
	weight = 6,
	isGun = 1,
	minimapMark = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 11) or cat_sample("$(xcom)/sound/sample2.cat", 11),
}

AddXcomItem {
	cost = 250,
	name = "LASER RIFLE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 0),
	pMap = pck_image("$(xcom)/units/floorob.pck", 0),
	health = 999,
	damage = 60,
	dDeviation = 100,
	importance = 7,
	width = 1,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 8, 8),
	damageType = 3,
	accuracy = {46, 65, 100},
	time = {12, 25, 50},
	autoShots = 3,
	weight = 8,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 11) or cat_sample("$(xcom)/sound/sample2.cat", 11),
}

AddXcomItem {
	cost = 300,
	name = "HEAVY LASER",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 6),
	pMap = pck_image("$(xcom)/units/floorob.pck", 6),
	health = 999,
	damage = 85,
	dDeviation = 100,
	importance = 12,
	width = 2,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 16, 8),
	damageType = 3,
	accuracy = {0, 50, 84},
	time = {0, 33, 75},
	weight = 18,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 11) or cat_sample("$(xcom)/sound/sample2.cat", 11),
}

AddXcomItem {
	cost = 70,
	name = "GRENADE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 19),
	pMap = pck_image("$(xcom)/units/floorob.pck", 19),      
	health = 999,
	damage = 50,
	dDeviation = 50,
	exploRange = 5,
	smokeRange = 5,
	smokeTime = 2,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	isGrenade = 1,
	weight = 3,
}

AddXcomItem {
	cost = 20,
	name = "SMOKE GRENADE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 20),
	pMap = pck_image("$(xcom)/units/floorob.pck", 20),
	health = 999,
	exploRange = 0,
	smokeRange = 5,
	smokeTime = 2,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	isGrenade = 1,
	weight = 3,
}

AddXcomItem {
	cost = 80,
	name = "PROXIMITY GRENADE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 21),
	pMap = pck_image("$(xcom)/units/floorob.pck", 21),
	health = 999,
	damage = 70,
	dDeviation = 50,
	exploRange = 6,
	smokeRange = 6,
	smokeTime = 2,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	isGrenade = 1,
	isProximityGrenade = 1,
	weight = 3,
}

AddXcomItem {
	cost = 105,
	name = "HIGH EXPLOSIVE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 22),
	pMap = pck_image("$(xcom)/units/floorob.pck", 22),
	health = 999,
	damage = 110,
	dDeviation = 50,
	exploRange = 6,
	smokeRange = 6,
	smokeTime = 2,
	importance = 4,
	width = 2,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	isGrenade = 1,
	isHighExplosive = 1,
	weight = 6,
}

AddXcomItem {
	cost = 100,
	name = "STUN ROD",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 26),
	pMap = pck_image("$(xcom)/units/floorob.pck", 26),
	health = 999,
	damage = 100,
	dDeviation = 100,
	importance = 6,
	width = 1,
	height = 3,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 80, 8),
	useTime = 25,
	damageType = 5,
	weight = 6,
	isHandToHand = 1,
	twoHanded = 1,
}

AddXcomItem {
	cost = 600,
	name = "HEAVY PLASMA",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 30),
	pMap = pck_image("$(xcom)/units/floorob.pck", 29),
	health = 999,
	importance = 10,
	width = 2,
	height = 3,
	ammo = {"HEAVY PLASMA CLIP"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 40, 8),
	accuracy = {50, 75, 110},
	time = {12, 30, 60},
	autoShots = 3,
	weight = 8,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 18) or cat_sample("$(xcom)/sound/sample2.cat", 18),
}

AddXcomItem {
	cost = 300,
	name = "HEAVY PLASMA CLIP",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 25),
	pMap = pck_image("$(xcom)/units/floorob.pck", 33),
	health = 999,
	damage = 115,
	dDeviation = 100,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 4,
	rounds = 35,
	disappear = 1,
	weight = 3,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 19) or cat_sample("$(xcom)/sound/sample2.cat", 19),
}

AddXcomItem {
	cost = 350,
	name = "PLASMA RIFLE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 31),
	pMap = pck_image("$(xcom)/units/floorob.pck", 30),
	health = 999,
	importance = 6,
	width = 1,
	height = 3,
	ammo = {"PLASMA RIFLE CLIP"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 48, 8),
	accuracy = {55, 86, 100},
	time = {12, 30, 60},
	autoShots = 3,
	weight = 5,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 18) or cat_sample("$(xcom)/sound/sample2.cat", 18),
}

AddXcomItem {
	cost = 100,
	name = "PLASMA RIFLE CLIP",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 41),
	pMap = pck_image("$(xcom)/units/floorob.pck", 33),
	health = 999,
	damage = 80,
	dDeviation = 100,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 4,
	rounds = 28,
	disappear = 1,
	weight = 3,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 19) or cat_sample("$(xcom)/sound/sample2.cat", 19),
}

AddXcomItem {
	cost = 170,
	name = "PLASMA PISTOL",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 32),
	pMap = pck_image("$(xcom)/units/floorob.pck", 31),
	health = 999,
	importance = 4,
	width = 1,
	height = 2,
	ammo = {"PLASMA PISTOL CLIP"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 104, 8),
	accuracy = {50, 65, 85},
	time = {10, 30, 60},
	autoShots = 3,
	weight = 3,
	isGun = 1,
	minimapMark = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 18) or cat_sample("$(xcom)/sound/sample2.cat", 18),
}

AddXcomItem {
	cost = 40,
	name = "PLASMA PISTOL CLIP",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 34),
	pMap = pck_image("$(xcom)/units/floorob.pck", 33),
	health = 999,
	damage = 52,
	dDeviation = 100,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 4,
	rounds = 26,
	disappear = 1,
	weight = 3,
	isAmmo = 1,
	reloadTime = 15,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 19) or cat_sample("$(xcom)/sound/sample2.cat", 19),
}

AddXcomItem {
	cost = 300,
	name = "SMALL LAUNCHER",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 37),
	pMap = pck_image("$(xcom)/units/floorob.pck", 36),
	health = 999,
	importance = 7,
	width = 2,
	height = 2,
	ammo = {"STUN MISSILE"},
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 64, 8),
	accuracy = {0, 65, 110},
	time = {0, 40, 75},
	weight = 10,
	isGun = 1,
	minimapMark = 1,
	twoHanded = 1,
	sound = cat_sample("$(xcom)/sound/sound1.cat", 53) or cat_sample("$(xcom)/sound/sample2.cat", 53),
}

AddXcomItem {
	cost = 80,
	name = "STUN MISSILE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 38),
	pMap = pck_image("$(xcom)/units/floorob.pck", 37),   
	health = 999,
	damage = 90,
	dDeviation = 100,
	exploRange = 5,
	smokeRange = 5,
	smokeTime = 2,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 5,
	rounds = 1,
	disappear = 1,
	weight = 3,
	reloadTime = 15,
}

AddXcomItem {
	cost = 125,
	name = "ALIEN GRENADE",
	pInv = pck_image("$(xcom)/units/bigobs.pck", 39),
	pMap = pck_image("$(xcom)/units/floorob.pck", 38),
	health = 999,
	damage = 90,
	dDeviation = 50,
	exploRange = 6,
	smokeRange = 6,
	smokeTime = 2,
	importance = 1,
	width = 1,
	height = 1,
	pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8),
	damageType = 2,
	isGrenade = 1,
	weight = 3,
	reloadTime = 15,
}

AddEquipment {
	Name = "Original X-Com Weapon Set",
	Layout = {
		{00, 00, "PISTOL CLIP"},
		{00, 01, "PISTOL"},
		{00, 06, "AUTO-CANNON"},
		{01, 00, "RIFLE CLIP"},
		{01, 01, "RIFLE"},
		{03, 01, "LASER PISTOL"},
		{02, 06, "AUTO-CANNON HE-AMMO"},
		{02, 07, "AUTO-CANNON I-AMMO"},
		{02, 08, "AUTO-CANNON AP-AMMO"},
		{04, 01, "LASER RIFLE"},
		{05, 01, "HEAVY LASER"},
		{05, 06, "HEAVY CANNON"},
		{08, 00, "PLASMA PISTOL CLIP"},
		{08, 01, "PLASMA PISTOL"},
		{09, 00, "PLASMA RIFLE CLIP"},
		{09, 01, "PLASMA RIFLE"},
		{07, 06, "CANNON HE-AMMO"},
		{07, 07, "CANNON I-AMMO"},
		{07, 08, "CANNON AP-AMMO"},
		{10, 00, "HEAVY PLASMA CLIP"},
		{10, 01, "HEAVY PLASMA"},
		{10, 06, "ROCKET LAUNCHER"},
		{13, 00, "ALIEN GRENADE"},
		{14, 00, "GRENADE"},
		{13, 06, "INCENDIARY ROCKET"},
		{15, 00, "PROXIMITY GRENADE"},
		{16, 02, "STUN ROD"},
		{12, 06, "LARGE ROCKET"},
		{16, 00, "SMOKE GRENADE"},
		{13, 02, "STUN MISSILE"},
		{13, 03, "SMALL LAUNCHER"},
		{14, 06, "SMALL ROCKET"},
		{18, 00, "HIGH EXPLOSIVE"},
		{18, 01, "ELECTRO FLARE"},
	}
}
