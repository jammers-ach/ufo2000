------------------------------------------------------------------------------
-- A place for standard x-com maps initialization.                          --
-- Editing this file is not recommended, better add new *.lua map init      --
-- files into 'newmaps' subdirectory along with your *.map files            --
--                                                                          --
-- Notes:                                                                   --
--   The entrance level must be a 3x3 maps, not randomised. ENTRYXX.map     --
--   Alien base has no corridors A_BASEXX.map                               --
--   Alien ship has no corridors AlshipXX.map                               --
--   Cruise ships cannot be randomised linertxx.map && linertbXX.map        --
--   T'leth levels must not be randomised. levelxx.map && cryptxx.map       --
--   xbasesXX.maps do not have any walls                                    --
------------------------------------------------------------------------------

AddXcomTerrain {
	Name = "Jungle",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/jungle.*",
	},
	Maps = {
		"$(xcom)/maps/jungle00.map",
		"$(xcom)/maps/jungle01.map",
		"$(xcom)/maps/jungle02.map",
		"$(xcom)/maps/jungle03.map",
		"$(xcom)/maps/jungle04.map",
		"$(xcom)/maps/jungle05.map",
		"$(xcom)/maps/jungle06.map",
		"$(xcom)/maps/jungle07.map",
		"$(xcom)/maps/jungle08.map",
		"$(xcom)/maps/jungle09.map",
		"$(xcom)/maps/jungle10.map",
		"$(xcom)/maps/jungle11.map"
	}
}

AddXcomTerrain {
	Name = "Farm",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/cultivat.*",
		"$(xcom)/terrain/barn.*"
	},
	Maps = {
		"$(xcom)/maps/culta00.map",
		"$(xcom)/maps/culta01.map",
		"$(xcom)/maps/culta02.map",
		"$(xcom)/maps/culta03.map",
		"$(xcom)/maps/culta04.map",
		"$(xcom)/maps/culta05.map",
		"$(xcom)/maps/culta06.map",
		"$(xcom)/maps/culta07.map",
		"$(xcom)/maps/culta08.map",
		"$(xcom)/maps/culta09.map",
		"$(xcom)/maps/culta10.map",
		"$(xcom)/maps/culta11.map",
		"$(xcom)/maps/culta12.map",
		"$(xcom)/maps/culta13.map",
		"$(xcom)/maps/culta14.map",
		"$(xcom)/maps/culta15.map",
		"$(xcom)/maps/culta16.map",
		"$(xcom)/maps/culta17.map",
		"$(xcom)/maps/culta18.map"
	}
}

AddXcomTerrain {
	Name = "Forest",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/forest.*"
	},
	Maps = {
		"$(xcom)/maps/forest00.map",
		"$(xcom)/maps/forest01.map",
		"$(xcom)/maps/forest02.map",
		"$(xcom)/maps/forest03.map",
		"$(xcom)/maps/forest04.map",
		"$(xcom)/maps/forest05.map",
		"$(xcom)/maps/forest06.map",
		"$(xcom)/maps/forest07.map",
		"$(xcom)/maps/forest08.map",
		"$(xcom)/maps/forest09.map",
		"$(xcom)/maps/forest10.map",
		"$(xcom)/maps/forest11.map"
	}
}

AddXcomTerrain {
	Name = "City",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/roads.*",
		"$(xcom)/terrain/urbits.*",
		"$(xcom)/terrain/urban.*",
		"$(xcom)/terrain/frniture.*"
	},
	Maps = {
		"$(xcom)/maps/urban00.map",
		"$(xcom)/maps/urban01.map",
		"$(xcom)/maps/urban02.map",
		"$(xcom)/maps/urban03.map",
		"$(xcom)/maps/urban04.map",
		"$(xcom)/maps/urban05.map",
		"$(xcom)/maps/urban06.map",
		"$(xcom)/maps/urban07.map",
		"$(xcom)/maps/urban08.map",
		"$(xcom)/maps/urban09.map",
		"$(xcom)/maps/urban14.map",
		"$(xcom)/maps/urban15.map",
		"$(xcom)/maps/urban16.map",
		"$(xcom)/maps/urban17.map",
		"$(xcom)/maps/urban18.map"
	},
	MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] = 1 end
			end
	
			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = 0 end
			end
	
			if (x and y) then
				map[x][y] = 2
			end
		end

		local function random_normal()
			return random {03, 04, 14, 15, 16, 17, 18}
		end	

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 2 and map[a][y] > 2 and map[x][b] > 2 and map[a][b] > 2) then
				map[x][y] = random {05, 06, 07, 08, 09}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_normal()
			end
		end

		add_roads(tmp.SizeX, tmp.SizeY, tmp.Mapdata)

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					random_double(i, j, tmp.Mapdata)
				end
			end
		end

		return tmp
	end		
}

AddXcomTerrain {
	Name = "Desert",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/desert.*"
	},
	Maps = {
		"$(xcom)/maps/desert00.map",
		"$(xcom)/maps/desert01.map",
		"$(xcom)/maps/desert02.map",
		"$(xcom)/maps/desert03.map",
		"$(xcom)/maps/desert04.map",
		"$(xcom)/maps/desert05.map",
		"$(xcom)/maps/desert06.map",
		"$(xcom)/maps/desert07.map",
		"$(xcom)/maps/desert08.map",
		"$(xcom)/maps/desert09.map",
		"$(xcom)/maps/desert10.map",
		"$(xcom)/maps/desert11.map"
	}
}

AddXcomTerrain {
	Name = "Mountain",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/mount.*"
	},
	Maps = {
		"$(xcom)/maps/mount00.map",
		"$(xcom)/maps/mount01.map",
		"$(xcom)/maps/mount02.map",
		"$(xcom)/maps/mount03.map",
		"$(xcom)/maps/mount04.map",
		"$(xcom)/maps/mount05.map",
		"$(xcom)/maps/mount06.map",
		"$(xcom)/maps/mount07.map",
		"$(xcom)/maps/mount08.map",
		"$(xcom)/maps/mount09.map",
		"$(xcom)/maps/mount10.map",
		"$(xcom)/maps/mount11.map",
		"$(xcom)/maps/mount12.map"
	}
}

AddXcomTerrain {
	Name = "Arctic",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/polar.*"
	},
	Maps = {
		"$(xcom)/maps/polar00.map",
		"$(xcom)/maps/polar01.map",
		"$(xcom)/maps/polar02.map",
		"$(xcom)/maps/polar03.map",
		"$(xcom)/maps/polar04.map",
		"$(xcom)/maps/polar05.map",
		"$(xcom)/maps/polar06.map",
		"$(xcom)/maps/polar07.map",
		"$(xcom)/maps/polar08.map",
		"$(xcom)/maps/polar09.map",
		"$(xcom)/maps/polar10.map",
		"$(xcom)/maps/polar11.map",
		"$(xcom)/maps/polar12.map",
		"$(xcom)/maps/polar13.map"
	}
}

AddXcomTerrain {
	Name = "Cydonia",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/mars.*",
		"$(xcom)/terrain/u_wall02.*"
	},
	Maps = {
		"$(xcom)/maps/mars00.map",
		"$(xcom)/maps/mars01.map",
		"$(xcom)/maps/mars02.map",
		"$(xcom)/maps/mars03.map",
		"$(xcom)/maps/mars04.map",
		"$(xcom)/maps/mars05.map",
		"$(xcom)/maps/mars06.map",
		"$(xcom)/maps/mars07.map",
		"$(xcom)/maps/mars08.map",
		"$(xcom)/maps/mars09.map",
		"$(xcom)/maps/mars10.map"
	}
}

AddXcomTerrain {
	Name = "Seabed",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/rocks.*",
		"$(tftd)/terrain/weeds.*",
		"$(tftd)/terrain/debris.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/seabed00.map",
		"$(tftd)/maps/seabed01.map",
		"$(tftd)/maps/seabed02.map",
		"$(tftd)/maps/seabed03.map",
		"$(tftd)/maps/seabed04.map",
		"$(tftd)/maps/seabed05.map",
		"$(tftd)/maps/seabed06.map",
		"$(tftd)/maps/seabed07.map",
		"$(tftd)/maps/seabed08.map",
		"$(tftd)/maps/seabed09.map",
		"$(tftd)/maps/seabed10.map",
		"$(tftd)/maps/seabed11.map",
		"$(tftd)/maps/seabed12.map"
	}
}


AddXcomTerrain {
	Name = "Pipes",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/pipes.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/pipes00.map",
		"$(tftd)/maps/pipes01.map",
		"$(tftd)/maps/pipes02.map",
		"$(tftd)/maps/pipes03.map",
		"$(tftd)/maps/pipes04.map",
		"$(tftd)/maps/pipes05.map",
		"$(tftd)/maps/pipes06.map",
		"$(tftd)/maps/pipes07.map",
		"$(tftd)/maps/pipes08.map",
		"$(tftd)/maps/pipes09.map",
		"$(tftd)/maps/pipes10.map",
		"$(tftd)/maps/pipes11.map"
	}
}

AddXcomTerrain {
	Name = "Plane",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/plane.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/plane00.map",
		"$(tftd)/maps/plane01.map",
		"$(tftd)/maps/plane02.map",
		"$(tftd)/maps/plane03.map",
		"$(tftd)/maps/plane04.map",
		"$(tftd)/maps/plane05.map",
		"$(tftd)/maps/plane06.map",
		"$(tftd)/maps/plane07.map",
		"$(tftd)/maps/plane08.map",
		"$(tftd)/maps/plane09.map",
		"$(tftd)/maps/plane10.map",
		"$(tftd)/maps/plane11.map",
		"$(tftd)/maps/plane12.map",
		"$(tftd)/maps/plane13.map",
		"$(tftd)/maps/plane14.map",
		"$(tftd)/maps/plane15.map",
		"$(tftd)/maps/plane16.map",
		"$(tftd)/maps/plane17.map",
		"$(tftd)/maps/plane18.map",
		"$(tftd)/maps/plane19.map",
		"$(tftd)/maps/plane20.map"
	}
}

AddXcomTerrain {
	Name = "Atlantis",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/atlantis.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/atlan00.map",
		"$(tftd)/maps/atlan01.map",
		"$(tftd)/maps/atlan02.map",
		"$(tftd)/maps/atlan03.map",
		"$(tftd)/maps/atlan04.map",
		"$(tftd)/maps/atlan05.map",
		"$(tftd)/maps/atlan06.map",
		"$(tftd)/maps/atlan07.map",
		"$(tftd)/maps/atlan08.map",
		"$(tftd)/maps/atlan09.map",
		"$(tftd)/maps/atlan10.map",
		"$(tftd)/maps/atlan11.map",
		"$(tftd)/maps/atlan12.map"
	}
}

AddXcomTerrain {
	Name = "Mu",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/mu.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/mu00.map",
		"$(tftd)/maps/mu01.map",
		"$(tftd)/maps/mu02.map",
		"$(tftd)/maps/mu03.map",
		"$(tftd)/maps/mu04.map",
		"$(tftd)/maps/mu05.map",
		"$(tftd)/maps/mu06.map",
		"$(tftd)/maps/mu07.map",
		"$(tftd)/maps/mu08.map",
		"$(tftd)/maps/mu09.map",
		"$(tftd)/maps/mu10.map",
		"$(tftd)/maps/mu11.map",
		"$(tftd)/maps/mu12.map",
		"$(tftd)/maps/mu13.map",
		"$(tftd)/maps/mu14.map"
	}
}

AddXcomTerrain {
	Name = "Galleon",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/rocks.*",
		"$(tftd)/terrain/asunk.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/gal01.map",
		"$(tftd)/maps/gal02.map",
		"$(tftd)/maps/gal03.map",
		"$(tftd)/maps/gal04.map",
		"$(tftd)/maps/gal05.map",
		"$(tftd)/maps/gal06.map",
		"$(tftd)/maps/gal07.map",
		"$(tftd)/maps/gal08.map",
		"$(tftd)/maps/gal09.map",
		"$(tftd)/maps/gal10.map",
		"$(tftd)/maps/gal11.map",
		"$(tftd)/maps/gal12.map",
		"$(tftd)/maps/gal13.map",
		"$(tftd)/maps/gal14.map",
		"$(tftd)/maps/gal15.map",
		"$(tftd)/maps/gal16.map"
	}
}

AddXcomTerrain {
	Name = "MSunk",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/msunk1.*",
		"$(tftd)/terrain/msunk2.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/msunk00.map",
		"$(tftd)/maps/msunk01.map",
		"$(tftd)/maps/msunk02.map",
		"$(tftd)/maps/msunk03.map",
		"$(tftd)/maps/msunk04.map",
		"$(tftd)/maps/msunk05.map",
		"$(tftd)/maps/msunk06.map",
		"$(tftd)/maps/msunk07.map",
		"$(tftd)/maps/msunk08.map",
		"$(tftd)/maps/msunk09.map",
		"$(tftd)/maps/msunk10.map",
		"$(tftd)/maps/msunk11.map",
		"$(tftd)/maps/msunk12.map",
		"$(tftd)/maps/msunk13.map"
	}
}

AddXcomTerrain {
	Name = "Volcanic",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/volc.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/volc00.map",
		"$(tftd)/maps/volc01.map",
		"$(tftd)/maps/volc02.map",
		"$(tftd)/maps/volc03.map",
		"$(tftd)/maps/volc04.map",
		"$(tftd)/maps/volc05.map",
		"$(tftd)/maps/volc06.map",
		"$(tftd)/maps/volc07.map",
		"$(tftd)/maps/volc08.map",
		"$(tftd)/maps/volc09.map",
		"$(tftd)/maps/volc10.map",
		"$(tftd)/maps/volc11.map"
	}
}

AddXcomTerrain {
	Name = "Coral",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/weeds.*",
		"$(tftd)/terrain/coral.*",
		"$(tftd)/terrain/rocks.*"
	},
	Maps = {
		"$(tftd)/maps/coral00.map",
		"$(tftd)/maps/coral01.map",
		"$(tftd)/maps/coral02.map",
		"$(tftd)/maps/coral03.map",
		"$(tftd)/maps/coral04.map",
		"$(tftd)/maps/coral05.map",
		"$(tftd)/maps/coral06.map",
		"$(tftd)/maps/coral07.map",
		"$(tftd)/maps/coral08.map",
		"$(tftd)/maps/coral09.map",
		"$(tftd)/maps/coral10.map",
		"$(tftd)/maps/coral11.map",
		"$(tftd)/maps/coral12.map"
	}
}

AddXcomTerrain {
	Name = "Artifact",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sand.*",
		"$(tftd)/terrain/rocks.*",
		"$(tftd)/terrain/weeds.*",
		"$(tftd)/terrain/pyramid.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/alart00.map",
		"$(tftd)/maps/alart01.map",
		"$(tftd)/maps/alart02.map",
		"$(tftd)/maps/alart03.map",
		"$(tftd)/maps/alart04.map",
		"$(tftd)/maps/alart05.map",
		"$(tftd)/maps/alart06.map",
		"$(tftd)/maps/alart07.map",
		"$(tftd)/maps/alart08.map",
		"$(tftd)/maps/alart09.map",
		"$(tftd)/maps/alart10.map",
		"$(tftd)/maps/alart11.map"
	}
}

AddXcomTerrain {
	Name = "Port",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sea.*",
		"$(tftd)/terrain/port01.*",
		"$(tftd)/terrain/port02.*"
	},
	Maps = {
		"$(tftd)/maps/port00.map",
		"$(tftd)/maps/port01.map",
		"$(tftd)/maps/port02.map",
		"$(tftd)/maps/port03.map",
		"$(tftd)/maps/port04.map",
		"$(tftd)/maps/port05.map",
		"$(tftd)/maps/port06.map",
		"$(tftd)/maps/port07.map",
		"$(tftd)/maps/port08.map",
		"$(tftd)/maps/port09.map",
		"$(tftd)/maps/port10.map",
		"$(tftd)/maps/port11.map",
		"$(tftd)/maps/port12.map",
		"$(tftd)/maps/port13.map",
		"$(tftd)/maps/port14.map",
		"$(tftd)/maps/port15.map",
		"$(tftd)/maps/port16.map",
		"$(tftd)/maps/port17.map",
		"$(tftd)/maps/port18.map",
		"$(tftd)/maps/port19.map",
		"$(tftd)/maps/port20.map"
	},
	MapGenerator = function(tmp)
		local function random_normal()
			if(math.random(1, 9) > 2) then
				return math.random(10, 16)
			else
				return math.random(0, 1)
			end
		end


		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if(map.Mapdata[x][y] ~= -1 and map.Mapdata[a][y] ~= -1 and map.Mapdata[x][b] ~= -1 and map.Mapdata[a][b] ~= -1) then
				if(y == map.SizeY - 1) then
					map.Mapdata[x][y] = math.random(17, 20)
				else
					map.Mapdata[x][y] = math.random(17, 19)
				end
				map.Mapdata[a][y] = -1
				map.Mapdata[x][b] = -1
				map.Mapdata[a][b] = -1
			end
		end	

		for i = 1, tmp.SizeY - 1 do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_normal()
			end
		end

		for i = 1, tmp.SizeY do
			tmp.Mapdata[tmp.SizeX][i] = math.random(2, 9)
		end

		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 1 do
				if (math.random(1, 13) > 9) then
					random_double(i, j, tmp)
				end
			end
		end

		return tmp
	end
}

------------------------------------------------------------------------------
-- The cargo must be pre-prepared in order to play in them. Randomisation   --
-- is impossible. Real map size is 70x30 for cargo ship, but we currently   --
-- have limitation 60x60, so map is cut to 60x30                            --
------------------------------------------------------------------------------

AddXcomTerrain {
	Name = "Cargo Ship",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/sea.*",
		"$(tftd)/terrain/cargo1.*",
		"$(tftd)/terrain/cargo2.*",
		"$(tftd)/terrain/xbits.*",
		"$(tftd)/terrain/cargo3.*"
	},
	Maps = {
		"$(tftd)/maps/cargo00.map",
		"$(tftd)/maps/cargo01.map",
		"$(tftd)/maps/cargo02.map"
	},
	MapGenerator = function(map)
		map.SizeX = 6
		map.SizeY = 3
		if (math.random(1, 2) ~= 2) then
			map.Mapdata = {
				{ 00, -1, -1, -1, -1, -1 },
				{ -1, -1, -1, -1, -1, -1 },
				{ -1, -1, -1, -1, -1, -1 },
			}
		else
			map.Mapdata = {
				{ 02, -1, 01, -1, -1, -1 },
				{ -1, -1, -1, -1, -1, -1 },
				{ -1, -1, -1, -1, -1, -1 },
			}
		end
		return map
	end
}

------------------------------------------------------------------------------
-- Alien Base, impossible to play (no passages)                             --
------------------------------------------------------------------------------
--[[  
AddXcomTerrain {
	Name = "Grunge",
	Tiles =	{
		"$(tftd)/terrain/blanks.*",
		"$(tftd)/terrain/grunge1.*",
		"$(tftd)/terrain/grunge2.*",
		"$(tftd)/terrain/grunge3.*",
		"$(tftd)/terrain/grunge4.*",
		"$(tftd)/terrain/ufobits.*"
	},
	Maps = {
		"$(tftd)/maps/grunge00.map",
		"$(tftd)/maps/grunge01.map",
		"$(tftd)/maps/grunge02.map",
		"$(tftd)/maps/grunge03.map",
		"$(tftd)/maps/grunge04.map",
		"$(tftd)/maps/grunge05.map",
		"$(tftd)/maps/grunge06.map",
		"$(tftd)/maps/grunge07.map",
		"$(tftd)/maps/grunge08.map",
		"$(tftd)/maps/grunge09.map",
		"$(tftd)/maps/grunge10.map",
		"$(tftd)/maps/grunge11.map",
		"$(tftd)/maps/grunge12.map",
		"$(tftd)/maps/grunge13.map",
		"$(tftd)/maps/grunge14.map",
		"$(tftd)/maps/grunge15.map",
		"$(tftd)/maps/grunge16.map"
	}
}
--]]
