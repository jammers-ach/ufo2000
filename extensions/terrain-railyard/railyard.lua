AddXcomTerrain {
	Name = "Railyard",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(tftd)/terrain/port01.*",
		"$(tftd)/terrain/port02.*",
		"$(xcom)/terrain/roads.*",
		"$(xcom)/terrain/urbits.*",
		"$(xcom)/terrain/frniture.*"

	},
	Maps = {
		"$(extension)/railyard00.map",
		"$(extension)/railyard01.map",
		"$(extension)/railyard02.map",
		"$(extension)/railyard03.map",
		"$(extension)/railyard04.map",
		"$(extension)/railyard05.map",
		"$(extension)/railyard06.map",
		"$(extension)/railyard07.map",
		"$(extension)/railyard08.map",
		"$(extension)/railyard09.map",
		"$(extension)/railyard10.map",
		"$(extension)/railyard11.map",
		"$(extension)/railyard12.map",
		"$(extension)/railyard13.map",
		"$(extension)/railyard14.map",
		"$(extension)/railyard15.map",
		"$(extension)/railyard16.map",
		"$(extension)/railyard17.map",
		"$(extension)/railyard18.map",
		"$(extension)/railyard19.map",
		"$(extension)/railyard20.map",
		"$(extension)/railyard21.map",
		"$(extension)/railyard22.map",
		"$(extension)/railyard23.map",
		"$(extension)/railyard24.map",
		"$(extension)/railyard25.map",
		"$(extension)/railyard26.map",
		"$(extension)/railyard27.map"
	},
MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y, z, w


			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] = random {02, 03, 04} end
			end

			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = 0 end
			end


			if (math.random(1, size_x) ~= 1) then
				z = math.random(1, size_x)
				for i = 1, size_y do map[z][i] = random {02, 03, 04} end

			end

			if (math.random(1, size_y) ~= 1) then
				w = math.random(1, size_y)
				for i = 1, size_x do map[i][w] = random {01, 08, 09} end
			end
	
	
			if (x and y) then
				map[x][y] = 7

			end

			if (z and y) then
				map[z][y] = 7

			end

			if (x and w) then
				map[x][w] = random {05, 06}

			end

			if (z and w) then
				map[z][w] = random {05, 06}

			end


		end

		local function random_normal()
			return random {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27}
		end	

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 10 and map[a][y] > 10 and map[x][b] > 10 and map[a][b] > 10) then
				map[x][y] = random {11, 12, 13, 14, 15}
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
			end
		end


		local function random_triple(x, y, map)
			local a = x + 1
			local b = y + 1
			local c = x + 2
			local d = y + 2

			if (map[x][y] > 10 and map[a][y] > 10 and map[x][b] > 10 and map[a][b] > 10) and map[c][y] > 15 and map[c][b] > 15 and map[x][d] > 15 and map[a][d] > 15 and map[c][d] > 15 then
				map[x][y] = 10
				map[a][y] = -1
				map[x][b] = -1
				map[a][b] = -1
				map[c][y] = -1
				map[x][d] = -1
				map[a][d] = -1
				map[c][b] = -1
				map[c][d] = -1
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
		
		for i = 1, tmp.SizeY - 2 do
			for j = 1, tmp.SizeX - 2 do
				if (math.random(1, 12) > 4) then
					random_triple(i, j, tmp.Mapdata)
				end
			end
		
		end


		return tmp
	end
}