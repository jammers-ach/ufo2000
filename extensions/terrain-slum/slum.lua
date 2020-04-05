AddXcomTerrain {
	Name = "Slum",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/roads.*",
		"$(xcom)/terrain/urbits.*",
		"$(xcom)/terrain/urban.*",
		"$(xcom)/terrain/frniture.*",
		"$(tftd)/terrain/deckc.*",
		"$(tftd)/terrain/linerc.*"

	},
	Maps = {
		"$(extension)/slum00.map",
		"$(extension)/slum01.map",
		"$(extension)/slum02.map",
		"$(extension)/slum03.map",
		"$(extension)/slum04.map",
		"$(extension)/slum05.map",
		"$(extension)/slum06.map",
		"$(extension)/slum07.map",
		"$(extension)/slum08.map",
		"$(extension)/slum09.map",
		"$(extension)/slum10.map",
		"$(extension)/slum11.map",
		"$(extension)/slum12.map",
		"$(extension)/slum13.map",
		"$(extension)/slum14.map",
		"$(extension)/slum15.map",
		"$(extension)/slum16.map",
		"$(extension)/slum17.map",
		"$(extension)/slum18.map",
		"$(extension)/slum19.map",
		"$(extension)/slum20.map",
		"$(extension)/slum21.map",
		"$(extension)/slum22.map",
		"$(extension)/slum23.map",
		"$(extension)/slum24.map",
		"$(extension)/slum25.map",
		"$(extension)/slum26.map",
		"$(extension)/slum27.map"

	},
MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y, z
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] =  random {05, 06} end
			end
	
			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = random {00, 01, 02} end
			end

	
			if (math.random(1, size_x) ~= 1) then
				z = math.random(1, size_y)
				for i = 1, size_y do map[z][i] =  random {07, 08} end
			end
	
			if (x and y) then
				map[x][y] =  random {03, 04}

			end

			if (z and y) then
				map[z][y] =  9
			end

		end

		local function random_normal()
			return random {23, 24, 25, 26, 27}
		end	

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 9 and map[a][y] > 9 and map[x][b] > 9 and map[a][b] > 9) then
				map[x][y] = random {10, 11}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end

		end

		local function random_triple(x, y, map)
			local b = y + 1
			if (map[x][y] > 21 and map[x][b] > 21) then
				map[x][y] = random {12, 13, 14, 15, 16}
				map[x][b] = -1

			end

		end

		local function random_quadruple(x, y, map)
			local b = y + 1
			local d = y + 2
			if (map[x][y] > 9 and map[x][b] > 21 and map[x][d] > 21) then
				map[x][y] = random {17, 18, 19, 20, 21, 22}
				map[x][b] = -1
				map[x][d] = -1
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
				if (math.random(1, 12) > 10) then
					random_double(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 1 do
				if (math.random(1, 12) > 8) then
					random_triple(i, j, tmp.Mapdata)
				end
			end
		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 8) then
					random_quadruple(i, j, tmp.Mapdata)
				end
			end
		end


		return tmp
	end		
}
