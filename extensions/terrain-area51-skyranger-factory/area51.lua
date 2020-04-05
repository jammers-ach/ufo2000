AddXcomTerrain {
	Name = "Area 51",
	Tiles =		{
		"$(xcom)/terrain/blanks.*",
		"$(extension)/area51a.*",
		"$(extension)/area51b.*",
		"$(extension)/area51c.*",
		"$(extension)/area51d.*",
		"$(extension)/area51e.*"
	},
	Maps = {
		"$(extension)/area00.map",
		"$(extension)/area01.map",
		"$(extension)/area02.map",
		"$(extension)/area03.map",
		"$(extension)/area04.map",
		"$(extension)/area05.map",
		"$(extension)/area06.map",
		"$(extension)/area07.map",
		"$(extension)/area08.map",
		"$(extension)/area09.map",
		"$(extension)/area10.map",
		"$(extension)/area11.map",
		"$(extension)/area12.map",
		"$(extension)/area13.map",
		"$(extension)/area14.map",
		"$(extension)/area15.map",
		"$(extension)/area16.map",
		"$(extension)/area17.map",
		"$(extension)/area18.map",
		"$(extension)/area19.map",
		"$(extension)/area20.map",
		"$(extension)/area21.map",
		"$(extension)/area22.map",
		"$(extension)/area23.map",
		"$(extension)/area24.map",
		"$(extension)/area25.map",
		"$(extension)/area26.map",
		"$(extension)/area27.map",
		"$(extension)/area28.map",
		"$(extension)/area29.map",
		"$(extension)/area30.map",
		"$(extension)/area31.map",
		"$(extension)/area32.map",
		"$(extension)/area33.map",
		"$(extension)/area34.map",
		"$(extension)/area35.map",
		"$(extension)/area36.map",
		"$(extension)/area37.map",
		"$(extension)/area38.map",
		"$(extension)/area39.map",
		"$(extension)/area40.map",
		"$(extension)/area41.map",
		"$(extension)/area42.map",
		"$(extension)/area43.map",
		"$(extension)/area44.map",
		"$(extension)/area45.map",
		"$(extension)/area46.map",
		"$(extension)/area47.map",
		"$(extension)/area48.map",
		"$(extension)/area49.map",
		"$(extension)/area50.map"
	},

MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y, w, z
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] = random {01, 04, 05, 06, 48} end
			end
	
			if (math.random(1, size_y) > 2) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = random {40, 41, 42} end
			end


			if (math.random(1, size_y) ~= 1) then
				w = math.random(1, size_y)
				for i = 1, size_x do map[i][w] = random {00, 07, 08, 49, 50} end
			end

			if (math.random(1, size_x) > 2) then
				z = math.random(1, size_x)
				for i = 1, size_y do map[z][i] = random {43, 44, 45} end
			end

			if (x and y) then
				map[x][y] = 3
			end

			
			if (x and w) then
				map[x][w] = 2
			end

			if (v and y) then
				map[v][y] = 3
			end
	
			if (v and w) then
				map[v][w] = 2
			end

			if (z and y) then
				map[z][y] = 47
			end

			
			if (z and w) then
				map[z][w] = 46
			end

			if (u and y) then
				map[u][y] = 47
			end

			
			if (u and w) then
				map[u][w] = 46
			end

		end

		local function random_normal()
			return random {25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39}
		end	

		local function random_double(x, y, map)
			local a = x + 1
			local b = y + 1
			if (map[x][y] > 8 and map[a][y] > 8 and map[x][b] > 8 and map[a][b] > 8 and  map[x][y] < 40 and map[a][y] < 40 and map[x][b] < 40 and map[a][b] < 40) then
				map[x][y] = random {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}
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

			if (map[x][y] > 8 and map[a][y] > 8 and map[x][b] > 8 and map[a][b] > 8 and map[c][y] > 24 and map[c][b] > 24 and map[x][d] > 24 and map[a][d] > 24 and map[c][d] > 24 and  map[x][y] < 40 and map[a][y] < 40 and map[x][b] < 40 and map[a][b] < 40 and map[c][y] < 40 and map[c][b] < 40 and map[x][d] < 40 and map[a][d] < 40 and map[c][d] < 40) then
				map[x][y] = random {09, 10, 11, 12}
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
				if (math.random(1, 12) > 2) then
					random_triple(i, j, tmp.Mapdata)
				end
			end
		
		end

		return tmp
	end		
}
