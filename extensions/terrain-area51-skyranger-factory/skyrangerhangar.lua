AddXcomTerrain {
	Name = "Skyranger Hangar",
	Tiles =		{
		"$(xcom)/terrain/blanks.*",
		"$(extension)/area51c.*",
		"$(extension)/area51d.*",
		"$(extension)/area51u.*",
		"$(extension)/area51p.*"
	},
	Maps = {
		"$(extension)/skyr00.map",
		"$(extension)/skyr01.map",
		"$(extension)/skyr02.map",
		"$(extension)/skyr03.map",
		"$(extension)/skyr04.map",
		"$(extension)/skyr05.map",
		"$(extension)/skyr06.map",
		"$(extension)/skyr07.map",
		"$(extension)/skyr08.map",
		"$(extension)/skyr09.map",
		"$(extension)/skyr10.map",
		"$(extension)/skyr11.map",
		"$(extension)/skyr12.map",
		"$(extension)/skyr13.map",
		"$(extension)/skyr14.map",
		"$(extension)/skyr15.map",
		"$(extension)/skyr16.map",
		"$(extension)/skyr17.map",
		"$(extension)/skyr18.map",
		"$(extension)/skyr19.map",
		"$(extension)/skyr20.map",
		"$(extension)/skyr21.map",
		"$(extension)/skyr22.map",
		"$(extension)/skyr23.map",
		"$(extension)/skyr24.map",
		"$(extension)/skyr25.map",
		"$(extension)/skyr26.map",
		"$(extension)/skyr27.map",
		"$(extension)/skyr28.map",
		"$(extension)/skyr29.map",
		"$(extension)/skyr30.map",
		"$(extension)/skyr31.map",
		"$(extension)/skyr32.map",
		"$(extension)/skyr33.map",
		"$(extension)/skyr34.map",
		"$(extension)/skyr35.map",
		"$(extension)/skyr36.map",
		"$(extension)/skyr37.map",
		"$(extension)/skyr38.map",
		"$(extension)/skyr39.map",
		"$(extension)/skyr40.map",
		"$(extension)/skyr41.map",
		"$(extension)/skyr42.map",
		"$(extension)/skyr43.map",
		"$(extension)/skyr44.map",
		"$(extension)/skyr45.map",
		"$(extension)/skyr46.map",
		"$(extension)/skyr47.map",
		"$(extension)/skyr48.map",
		"$(extension)/skyr49.map",
		"$(extension)/skyr50.map",
		"$(extension)/skyr51.map",
		"$(extension)/skyr52.map",
		"$(extension)/skyr53.map",
		"$(extension)/skyr54.map"

	},

	MapGenerator = function(map)

		if (map.SizeX ~= 6) then map.SizeX = 6 end
		
		if (map.SizeY ~= 6) then map.SizeY = 6 end

		if ((map.SizeX == 6) and (map.SizeY == 6)) then


		map.Mapdata = {
                   		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                  		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
           		    }
	          end

		-- General disposition of the map--
		-- A B I  I  B A --
		-- D E H1 H1 F C --
 		-- D E H1 H1 F C --
		-- D E H1 H1 F C --
		-- D E H2 H2 F C --
		-- A B J  J  B A --

		-- function to fill the battlefield with 10x10 maps --

		local function random_normal() 
			return math.random (40, 46)
		end

		-- function to fill H slots with 10x20 Skyranger maps --
			
		local function random_double(x, y, map) 
			local a = x + 1
			if (map[x][y] > 39 and map[x][y] < 47 and map[a][y] > 39 and map[a][y] < 47) 				then
				map[x][y] = random {50, 51, 52, 53, 54}
				map[a][y] = -1
			end
		end

		-- if there's no 20x20 maps on the H1 slots it adds a 20x20 map with a Skyranger there  --

		local function skyranger(x, y, map) 
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 39 and map[x][y] < 47 and map[a][y] > 31 and map[a][y] < 36 and map[x][b] > 39 and map[x][b] < 47 and map[a][b] > 31 and map[a][b] < 36) then
				map[x][y] = 49
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end

		-- command to fill all the battlefield with 10x10 maps --

		for i = 1, map.SizeY - 1 do
			for j = 1, map.SizeX do
				map.Mapdata[i][j] = random_normal()
			end
		end

		-- command to fill C column with its specific maps --

		for i = 1, map.SizeX do
			map.Mapdata[i][map.SizeY] = random {12, 13, 14, 15}
		end

		-- command to fill D column with its specific maps --

		for i = 1, map.SizeX do
			map.Mapdata[i][1] = random {16, 17, 18, 19}
		end

		-- command to fill E column with its specific maps --

		for i = 1, map.SizeX do
			map.Mapdata[i][2] = random {20, 21, 22, 23, 30, 31}
		end

		-- command to fill F column with its specific maps --

		for i = 1, map.SizeX do
			map.Mapdata[i][map.SizeY - 1] = random {24, 25, 26, 27, 28, 29}
		end

		-- command to fill I line with its specific maps --

		for i = 1, map.SizeY do
			map.Mapdata[1][i] = random {36, 37, 38, 39}
		end

		-- command to fill J line with its specific maps --

		for i = 1, map.SizeY do
			map.Mapdata[map.SizeX][i] = random {32, 33, 34, 35}
		end

		-- adds 20x10 maps to I line --

			if (math.random(1, 12) > 8) then
					map.Mapdata[1][3] = random {47, 48}
					map.Mapdata[1][4] = -1
			end

		-- sets specific maps for the A and B corner slots --

		for i = 1, map.SizeX do
			map.Mapdata[1][1] = random {00, 08}
		end

		for i = 1, map.SizeX do
			map.Mapdata[1][2] = 4
		end

		for i = 1, map.SizeX do
			map.Mapdata[1][map.SizeY - 1] = 5
		end

		for i = 1, map.SizeX do
			map.Mapdata[1][map.SizeY] = random {01, 11}
		end

		for i = 1, map.SizeX do
			map.Mapdata[map.SizeX][1] = random {02, 10}
		end


		for i = 1, map.SizeX do
			map.Mapdata[map.SizeX][2] = 6
		end

		for i = 1, map.SizeX do
			map.Mapdata[map.SizeX][map.SizeY - 1] = 7
		end

		for i = 1, map.SizeX do
			map.Mapdata[map.SizeX][map.SizeY] = random {03, 09}
		end

		-- uses random_double function to add 20x20 maps to H1 slots --

		for i = 1, map.SizeY - 1 do
			for j = 1, map.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					random_double(i, j, map.Mapdata)
				end
			end
		end

		-- uses skyranger function to add 20x20 Skyranger map to H2 and J slots --

		for i = 1, map.SizeY - 1 do
			for j = 1, map.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					skyranger(i, j, map.Mapdata)
				end
			end
		end

		return map

	end
}
