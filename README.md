# iw4sptool // a multipurpose tool for call of duty : modern warfare 2 (2009)

<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/87bc22c7-faae-4a56-b60a-f30aa1ed4785" />

<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/95d680ea-eea0-4151-80bf-c2a47d522edb" />


## Features
- collision drawing
- entity drawing
- debug information
- custom console
- toggle triggers

<img width="1920" height="116" alt="image" src="https://github.com/user-attachments/assets/a9e79627-8c9c-46fa-9ca7-a5a16e499533" />


## Notes
- Only for MW2 steam version
  <img width="801" height="25" alt="image" src="https://github.com/user-attachments/assets/c7ede4cc-cec4-45e7-af67-4136a2c6ed8b" />

## Usage
- The .asi file goes to the "miles" folder located in the mw2 root directory
- The console can be opened with the ~ or ` key (the same as in cod4)
- The console output window can be opened by pressing left shift and right shift at the same time
- All commands start with the following prefixes:
    - cm_ (clip map)
    - pm_ (player movement)
- cm_showCollisionFilter expects the name of a material (e.g. "clip") and using it without an argument will clear the render queue
- cm_showEntities expects the name of a classname (e.g. "trigger script") and using it without an argument will clear the render queue
- you can include multiple arguments on cm_showCollisionFilter and cm_showEntities by using a space-separated list, (e.g. "clip caulk mantle")
- using the "all" argument on cm_showCollisionFilter and cm_showEntities will select *ALL* relevant items
- brushes are highlighted with green
- terrain is highlighted with blue
- entities are highlighted with red
