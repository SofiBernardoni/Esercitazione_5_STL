#include "Utils.hpp"
#include "PolygonalMesh.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace PolygonalLibrary {
bool ImportMesh(const string& filepath,
                PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                       mesh))
    {
        return false;
    }

    else
    {

        //test sulla lunghezza dei lati

        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> edge = mesh.Cell2DEdges[c];
            size_t n = edge.size();

            for(unsigned int e = 0; e < n; e++)
            {
                const unsigned int origin = mesh.Cell1DVertices[edge[e]][0];
                const unsigned int end = mesh.Cell1DVertices[edge[e]][1];

                if (end-origin==0)
                {
                    cout << "wrong mesh" << endl;
                }

            }
        }

        // test area dei poligoni diversa da zero

        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> vertices = mesh.Cell2DVertices[c];
            size_t n= vertices.size();
            double somma=0;
            for(int i=0; i<n; i++)
            {
                int j = (i + 1) % n;
                somma+=((mesh.Cell0DCoordinates[vertices[i]][0]*mesh.Cell0DCoordinates[vertices[j]][1]) - (mesh.Cell0DCoordinates[vertices[j]][0]*mesh.Cell0DCoordinates[vertices[i]][1]));
            }

            somma = abs(somma)/2;
            if(somma< 1e-12)
            {
                cerr << "poligono di area degenere";
            }
        }


    }


    return true;

}
// ***************************************************************************
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front(); //elimina il primo elemento della lista, l'intestazione

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        char separator;
        unsigned int id;
        unsigned int marker;
        Vector2d coord;

        converter >>  id >> separator >> marker >>separator >> coord(0) >>separator >> coord(1);

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);

        if( marker != 0)
        {
            //            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            //                mesh.Cell0DMarkers.insert({marker, {id}});
            //            else
            //                mesh.Cell0DMarkers[marker].push_back(id);


            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }

    }
    file.close();
    return true;
}
// ***************************************************************************
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        char  separator;
        unsigned int id;
        unsigned int marker;
        Vector2i vertices;

        converter >>  id >>separator >> marker >>separator >> vertices(0) >> separator >> vertices(1);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);
        if( marker != 0)
        {
            //            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            //                mesh.Cell0DMarkers.insert({marker, {id}});
            //            else
            //                mesh.Cell0DMarkers[marker].push_back(id);


            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }



    }

    file.close();

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        char separator;
        unsigned int marker;
        unsigned int NumVer;
        unsigned int NumEdg;

        converter >> id >> separator >> marker >> separator >> NumVer;
        vector<unsigned int> vertices(NumVer);
        for( unsigned int i=0; i<NumVer; i++)
        {
            converter >> separator >> vertices[i];
        }
        converter >> separator >> NumEdg;
        vector<unsigned int> edges(NumEdg);
        for(unsigned int i=0; i<NumEdg; i++)
        {
            converter >> separator >> edges[i];
        }

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);
        if( marker != 0)
        {
            //            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            //                mesh.Cell0DMarkers.insert({marker, {id}});
            //            else
            //                mesh.Cell0DMarkers[marker].push_back(id);


            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }
    file.close();
    return true;
}

}
