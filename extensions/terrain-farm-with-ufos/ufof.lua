AddXcomTerrain {
	Name = "Farm with UFOs",
	Tiles = {
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/u_ext02.*",
		"$(xcom)/terrain/u_wall02.*",
		"$(xcom)/terrain/U_bits.*",
		"$(xcom)/terrain/u_disec2.*",
		"$(xcom)/terrain/u_oper2.*",
		"$(xcom)/terrain/u_pods.*",
		"$(xcom)/terrain/cultivat.*",
		"$(xcom)/terrain/barn.*",
		"$(xcom)/terrain/ufo1.*"
	},
	Maps = {
		"$(extension)/ufof00.map",
		"$(extension)/ufof01.map",
		"$(extension)/ufof02.map",
		"$(extension)/ufof03.map",
		"$(extension)/ufof04.map",
		"$(extension)/ufof05.map",
		"$(extension)/ufof06.map",
		"$(extension)/ufof07.map",
		"$(extension)/ufof10.map",
		"$(extension)/ufof11.map",
		"$(extension)/ufof12.map",
		"$(extension)/ufof13.map",
		"$(extension)/ufof14.map",
		"$(extension)/ufof15.map",
		"$(extension)/ufof16.map",
		"$(extension)/ufof17.map",
		"$(extension)/ufof18.map",
		"$(extension)/ufof19.map",
		"$(extension)/ufof20.map",
		"$(extension)/ufof21.map",
		"$(extension)/ufof22.map",
		"$(extension)/ufof23.map",
		"$(extension)/ufof24.map",
		"$(extension)/ufof25.map",
		"$(extension)/ufof26.map",
		"$(extension)/ufof27.map",
		"$(extension)/ufof28.map"


	},

	MapGenerator = function(tmp)
		local function add_ufos(size_x, size_y, map)
			
			local random_number = math.random (1, 8)

			if random_number == 1 then
				local x = math.random(1, size_x)
				local y = math.random(1, size_y)
				map[x][y] = 1
			end

			if random_number == 2 then
				local x = math.random(1, (size_x - 2))
				local y = math.random(1, (size_y - 2))
				local a = (x + 1)
				local b = (y + 1)
				map[x][y] = 2
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
			end

			if random_number == 3 then
				local x = math.random(1, (size_x - 2))
				local y = math.random(1, (size_y - 2))
				local a = (x + 1)
				local b = (y + 1)
				map[x][y] = 3
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
			end


			if random_number == 4 then
				local x = math.random(1, (size_x - 2))
				local y = math.random(1, (size_y - 2))
				local a = (x + 1)
				local b = (y + 1)
				map[x][y] = 4
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
			end

			
			if random_number == 5 then
				local x = math.random(1, (size_x - 3))
				local y = math.random(1, (size_y - 2))
				local a = (x + 1)
				local b = (y + 1)
				local c = (x + 2)
				map[x][y] = 5
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
				map[c][y] = -1
				map[c][b] = -1
				
			end

			if random_number == 6 then
				local x = math.random(1, (size_x - 3))
				local y = math.random(1, (size_y - 3))
				local a = (x + 1)
				local b = (y + 1)
				local c = (x + 2)
				local d = (y + 2)
				map[x][y] = 6
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
				map[c][y] = -1
				map[c][b] = -1
				map[x][d] = -1
				map[a][d] = -1
				map[c][d] = -1
			end

			if random_number == 7 then
				local x = math.random(1, (size_x - 3))
				local y = math.random(1, (size_y - 2))
				local a = (x + 1)
				local b = (y + 1)
				local c = (x + 2)
				map[x][y] = 7
				map[x][b] = -1
				map[a][y] = -1
				map[a][b] = -1
				map[c][y] = -1
				map[c][b] = -1

			end

			if random_number == 8 then
				local x = math.random(1, size_x)
				local y = math.random(1, size_y)
				map[x][y] = 0
			end

		end

						
		local function random_normal()
			return random {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28}

		end

		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_normal()
			end
		end

		add_ufos(tmp.SizeX, tmp.SizeY, tmp.Mapdata)


		return tmp
	end

}