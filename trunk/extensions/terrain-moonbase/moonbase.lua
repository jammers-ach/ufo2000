AddTileset {
    Name = "moonbase",
    Tiles = {
        [1] = {
            IsometricImage = png_image("$(extension)/moonbase/32x40-001.png"),
            Shape = [[
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
            ]],
            MinimapImage = [[
                408040,408040,408040,408040,
                408040,408040,408040,408040,
                408040,408040,408040,408040,
                408040,408040,408040,408040,
            ]],
            TU_Walk = 4,
        },
        [2] = {
            IsometricImage = png_image("$(extension)/moonbase/32x40-029.png"),
            Shape = [[
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
                FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,FFFF,
            ]],
            MinimapImage = [[
                808080,808080,808080,808080,
                808080,808080,808080,808080,
                808080,808080,808080,808080,
                808080,808080,808080,808080,
            ]],
            TU_Walk = -1,
            Stop_LOS = 1,
        },
        [3] = {
            IsometricImage = png_image("$(extension)/moonbase/32x40-018.png"),
            Shape = [[
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
                0000,0000,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,3FFC,0000,0000,
            ]],
            MinimapImage = [[
                FF00FF,FF00FF,FF00FF,FF00FF,
                FF00FF,FF4000,FF4000,FF00FF,
                FF00FF,FF4000,FF4000,FF00FF,
                FF00FF,FF00FF,FF00FF,FF00FF,
            ]],
            TU_Walk = -1,
            HE_Strength = 20,
        },
    }
}

AddTerrain {
    Name = "Moon Base (test version)",
    Tiles = {
        "moonbase",
    },
    Maps = {
        "$(extension)/moonbase00.map",
        "$(extension)/moonbase01.map",
    }
}
