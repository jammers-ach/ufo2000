AddXcomTerrain {
	Name = "Polis",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/roads.*",
		"$(xcom)/terrain/urbits.*",
		"$(xcom)/terrain/urban.*",
		"$(xcom)/terrain/frniture.*"
	},
	Maps = {
		"$(extension)/polis01.map",
		"$(extension)/polis02.map",
		"$(extension)/polis03.map",
		"$(extension)/polis04.map",
		"$(extension)/polis05.map",
		"$(extension)/polis06.map",
		"$(extension)/polis07.map",
		"$(extension)/polis08.map",
		"$(extension)/polis10.map",
		"$(extension)/polis11.map",
		"$(extension)/polis12.map",
		"$(extension)/polis13.map",
		"$(extension)/polis14.map",
		"$(extension)/polis15.map",
		"$(extension)/polis16.map",
		"$(extension)/polis17.map",
		"$(extension)/polis18.map",
		"$(extension)/polis19.map",
		"$(extension)/polis20.map",
		"$(extension)/polis21.map",
		"$(extension)/polis22.map",
		"$(extension)/polis23.map",
		"$(extension)/polis24.map",
		"$(extension)/polis25.map",
		"$(extension)/polis26.map",
		"$(extension)/polis27.map",
		"$(extension)/polis28.map",
		"$(extension)/polis29.map",
		"$(extension)/polis30.map",
		"$(extension)/polis31.map",
		"$(extension)/polis32.map",
		"$(extension)/polis33.map"

	},

	MapGenerator = function(map)

		if ((map.SizeX == 4) and (map.SizeY == 4)) then
		local function random_normal()
			return random {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25}
		end
	
		for i = 1, map.SizeY do
			for j = 1, map.SizeX do
				map.Mapdata[i][j] = random_normal()
			end
		end

		return map
	end
		
		if ((map.SizeX == 5) and (map.SizeY == 5)) then
		local function random_normal()
			return random {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}
		end

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 2 and map[a][y] > 2 and map[x][b] > 2 and map[a][b] > 2) then
				map[x][y] = random {28, 29, 30, 31, 32, 33}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end

		for i = 1, map.SizeY do
			for j = 1, map.SizeX do
				map.Mapdata[i][j] = random_normal()
			end
		end

		for i = 1, map.SizeY - 1 do
			for j = 1, map.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					random_double(i, j, map.Mapdata)
				end
			end
		end

		return map
	end
		if ((map.SizeX == 6) and (map.SizeY == 6)) then
   			  local map_number = math.random(1, 8)

           if map_number == 1 then
               	map. Mapdata = {
		{ 01, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }

           elseif map_number == 2 then
               map.Mapdata = {
		{ 02, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           elseif map_number == 3 then
               map.Mapdata = {
		{ 03, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           elseif map_number == 4 then
               map.Mapdata = {
		{ 04, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           elseif map_number == 5 then
               map.Mapdata = {
		{ 05, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           elseif map_number == 6 then
               map.Mapdata = {
		{ 06, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }   
    
           elseif map_number == 7 then
               map.Mapdata = {
		{ 07, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               } 
      
           elseif map_number == 8 then
               map.Mapdata = {
		{ 08, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           end

           return map
           end
           end
}