Matpath = "../../Materials/Map/"
import os

texList = os.listdir(".")

print(texList)

for i in range(len(texList)):
    if ".xml" not in os.path.splitext(texList[i]):
        file = os.path.splitext(texList[i])[0]
        with open(file+".xml", 'w') as f:
            if os.path.isfile("./"+file+"bump.jpg"):
                f.write("""<material>
        <technique name="Techniques/LitOpaque.xml" />
        <shader vsdefines="LIGHTMAP Normal" psdefines="LIGHTMAP Normal" />
        <texture unit="diffuse" name="Textures/Map/"""+texList[i]+"""" />
        <texture unit="normal" name="Textures/Map/"""+file+"""bump.jpg" />
        <parameter name="MatSpecColor" value="0.5 0.5 0.5 16" />
    </material>""")
                print("Wrote normal texture for: " +file)
            else:
                f.write("""<material>
        <technique name="Techniques/LitOpaque.xml" />
        <shader vsdefines="LIGHTMAP" psdefines="LIGHTMAP" />
        <texture unit="diffuse" name="Textures/Map/"""+texList[i]+"""" />
    </material>""")
                print("Wrote diffuse texture for: " +texList[i])
                print("Made material for: " + file)
