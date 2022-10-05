import lattice_tool as lt
import matplotlib.pyplot as plt

Lx = 5
Ly = 4
lat = "honeycomb"
kagome=lt.lattice(lat,Lx,Ly)
print(kagome.type)
#print(kagome.translation_vecs())
coords = kagome.lattice_coords()
for j in coords:
    plt.scatter(coords[j][0], coords[j][1], c='k')
    plt.text(coords[j][0]+0.05,coords[j][1],str(j),fontsize=8)

kagome = lt.NN_bonds(lat,Lx,Ly,"open")
bonds = kagome.bonds()
for bond in bonds:
    i = bond[0]
    j = bond[1]
    plt.plot([coords[i][0],coords[j][0]],\
            [coords[i][1],coords[j][1]],lw=1.5,c='orange')
plt.axis("equal")
plt.axis("off")
plt.show()
