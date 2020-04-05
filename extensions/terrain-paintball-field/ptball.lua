--------------------------------------------------------------------------
--  PAINTBALL FIELD		c 2005 by Violazr		V 1.0	--
--------------------------------------------------------------------------
--  Visit the UFO2000 Map Depot:  http://area51.xcomufo.com/depot.htm 	--
--------------------------------------------------------------------------



AddXcomTerrain {

	Name = "Paintball Field",

	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(extension)/pbbase.*",
		"$(extension)/obs_red.*",
		},


	Maps = {
		"$(extension)/pb_00.map",
		"$(extension)/pb_01.map",
		"$(extension)/pb_02.map",
		"$(extension)/pb_03.map",
		"$(extension)/pb_04.map",
		"$(extension)/pb_05.map",
		"$(extension)/pb_06.map",
		"$(extension)/pb_07.map",
		"$(extension)/pb_08.map",
		"$(extension)/pb_09.map",
		"$(extension)/pb_10.map",
		"$(extension)/pb_11.map",
		"$(extension)/pb_12.map",
		"$(extension)/pb_13.map",
		"$(extension)/pb_14.map",
		"$(extension)/pb_15.map",
		"$(extension)/pb_16.map",
		"$(extension)/pb_17.map",
		"$(extension)/pb_18.map",
		"$(extension)/pb_19.map",
		"$(extension)/pb_20.map",
		},
		


	MapGenerator = function(map)

		map.Mapdata = {
                	   		{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
         	         		{ -1, -1, -1, -1, -1, -1 },
                	   		{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
           			}


		local function random_0()
			return random {00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}
		end	


		for i = 1, map.SizeY do
			for j = 1, map.SizeX do

				map.Mapdata[i][j] = random_0()
				
				if i~=1 then
					while map.Mapdata[i][j] == map.Mapdata[i-1][j]
					do
						map.Mapdata[i][j] = random_0()
					end
				end

				if j~=1 then
					while map.Mapdata[i][j] == map.Mapdata[i][j-1]
					do
						map.Mapdata[i][j] = random_0()
					end
				end

			end
		end



		if ((map.SizeX == 4 and map.SizeY == 4) and math.random(0,2) == 2) then
			map.Mapdata[2][1] = 20
			map.Mapdata[3][4] = 20
		end


		if (map.SizeX == 5 and map.SizeY == 5) then
			map.Mapdata[3][3] = 20
		end


		if ((map.SizeX == 6 and map.SizeY == 6) and math.random(0,1) == 1) then
			map.Mapdata[2][2] = 20
			map.Mapdata[5][5] = 20
			map.Mapdata[2][5] = 20
			map.Mapdata[5][2] = 20
		end



   		return map
   	end
}