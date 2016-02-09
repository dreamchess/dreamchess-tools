/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <tristripper/tri_stripper.h>

using namespace std;

typedef triangle_stripper::index idx;
typedef vector <idx *> idx_pointers;

vector <float> vertices;
vector <float> normals;
vector <float> tex_coords;
vector <idx> indices;

void dcm_load(char *filename)
{
    int version;
    int vertices_nr, faces_nr;
    string str;
    vector <float> vertices_org;
    vector <float> normals_org;

    ifstream f(filename);

    if (!f)
    {
        cout << "Could not open " << filename << endl;
        exit(1);
    }

    f >> str;

    if (str != "DCM")
    {
        cout << "Not a DCM file." << endl;
        exit(1);
    }

    f >> version;

    if (version != 0)
    {
        cout << "DCM version " << version << " is not supported." << endl;
        exit(1);
    }

    f >> vertices_nr;

    cout << "File contains:" << endl;
    cout << vertices_nr << " vertices" << endl;

    while (vertices_nr--)
    {
        for (int i = 0; i < 3; i++)
        {
            float v;
            f >> v;
            vertices_org.push_back(v);
        }
        for (int i = 0; i < 3; i++)
        {
            float v;
            f >> v;
            normals_org.push_back(v);
        }
    }

    f >> faces_nr;

    cout << faces_nr << " textured triangles" << endl << endl;

    for (int face = 0; face < faces_nr; face++)
    {
        for (int i = 0; i < 3; i++)
        {
            int index;
            f >> index;
            for (int j = 0; j < 3; j++)
                vertices.push_back(vertices_org[index * 3 + j]);
            for (int j = 0; j < 3; j++)
                normals.push_back(normals_org[index * 3 + j]);
        }
        for (int i = 0; i < 6; i++)
        {
            float coord;
            f >> coord;
            tex_coords.push_back(coord);
        }

        indices.push_back(face * 3);
        indices.push_back(face * 3 + 1);
        indices.push_back(face * 3 + 2);
    }

    cout << "Converted to:" << endl;
    cout << vertices.size() / 3 << " textured vertices" << endl;
    cout << indices.size() / 3 << " triangles" << endl << endl;

    f.close();
}

struct compare_vertices
{
    bool operator () (unsigned int a, unsigned int b) const
    {
        for (int i = 0; i < 3; i++)
        {
            if (vertices[a * 3 + i] < vertices[b * 3 + i])
                return true;

            if (vertices[a * 3 + i] > vertices[b * 3 + i])
                return false;
        }

        for (int i = 0; i < 3; i++)
        {
            if (normals[a * 3 + i] < normals[b * 3 + i])
                return true;

            if (normals[a * 3 + i] > normals[b * 3 + i])
                return false;
        }

        for (int i = 0; i < 2; i++)
        {
            if (tex_coords[a * 2 + i] < tex_coords[b * 2 + i])
                return true;

            if (tex_coords[a * 2 + i] > tex_coords[b * 2 + i])
                return false;
        }

        return false;
    }
};

void normalize()
{
    using namespace std;

    typedef map <idx, idx_pointers, compare_vertices> vert_map;
    vector <float> vertices_new;
    vector <float> normals_new;
    vector <float> tex_coords_new;
    vert_map vertex_map;

    for (unsigned int i = 0; i < indices.size(); i++)
        vertex_map[indices[i]].push_back(&indices[i]);

    // Iterate over all vertices in the map.
    for (vert_map::const_iterator it = vertex_map.begin();
            it != vertex_map.end(); it++)
    {
        // The old index of the vertex.
        const idx v_pos = (*it).first;
        // Pointers to all indices of this same vertex.
        const idx_pointers &idx_ptr = (*it).second;
        // The new index for this vertex.
        const idx index_new = vertices_new.size() / 3;

        // Copy vertex to new arrays.
        for (int i = 0; i < 3; i++)
            vertices_new.push_back(vertices[v_pos * 3 + i]);

        for (int i = 0; i < 3; i++)
            normals_new.push_back(normals[v_pos * 3 + i]);

        for (int i = 0; i < 2; i++)
            tex_coords_new.push_back(tex_coords[v_pos * 2 + i]);

        // Iterate over all pointers and change the index to the new index.
        for (idx_pointers::const_iterator idx_it = idx_ptr.begin();
                idx_it != idx_ptr.end(); idx_it++)
            **idx_it = index_new;
    }

    vertices = vertices_new;
    normals = normals_new;
    tex_coords = tex_coords_new;

    cout << "Normalized to:" << endl;
    cout << vertices.size() / 3 << " textured vertices" << endl;
    cout << indices.size() / 3 << " triangles" << endl << endl;
}

using namespace triangle_stripper;

void dcm_write(char *filename, primitive_vector &vector)
{
    int triangles = 0;
    int strips = 0;

    for (unsigned int i = 0; i < vector.size(); i++)
    {
        switch (vector[i].Type)
        {
        case TRIANGLES:
            triangles += vector[i].Indices.size();
            break;
        case TRIANGLE_STRIP:
            strips++;
        }
    }

    cout << "Stripped to:" << endl;
    cout << vertices.size() / 3 << " textured vertices" << endl;
    cout << triangles << " triangles" << endl;
    cout << strips << " triangle strips" << endl;

    ofstream f(filename);

    f << "DCM 0100" << endl;
    f << vertices.size() / 3 << endl;
    for (unsigned int i = 0; i < vertices.size(); i++)
        f << vertices[i] << endl;
    for (unsigned int i = 0; i < normals.size(); i++)
        f << normals[i] << endl;
    for (unsigned int i = 0; i < tex_coords.size(); i++)
        f << tex_coords[i] << endl;
    f << vector.size() << endl;

    for (unsigned int i = 0; i < vector.size(); i++)
    {
        switch (vector[i].Type)
        {
        case TRIANGLES:
            f << "TRIANGLES" << endl;
            break;
        case TRIANGLE_STRIP:
            f << "STRIP" << endl;
        }

        f << vector[i].Indices.size() << endl;

        for (unsigned int j = 0; j < vector[i].Indices.size(); j++)
            f << vector[i].Indices[j] << endl;
    }

    f.close();
}

int main(int argc, char *argv[])
{
    primitive_vector primitive;
    if (argc < 4)
    {
        cout << "Usage: dcmstrip <input.dcm> <output.dcm> <cache_size>" << endl;
        exit(0);
    }
    dcm_load(argv[1]);
    normalize();
    tri_stripper stripper(::indices);
    stripper.SetCacheSize(atoi(argv[3]));
    stripper.Strip(&primitive);
    dcm_write(argv[2], primitive);
}
