#include <ugltf/ugltf2.h>

// THis is not included with ugltf, but was
// downloaded by the CMakeLists.txt file specifically for building
// this app
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#define INDENT "  "

#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <spdlog/fmt/fmt.h>



int is_number(const std::string &s)
{
    if( !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit) )
    {
        return std::stoi(s);
    }
    return -1;
}

std::string readResourceASCII(const std::filesystem::path &path)
{
    std::ifstream t( path );
    if(!t)
    {
        throw std::invalid_argument( std::string("File does not exist: ") + path.string());
    }
    std::string asciiSrc((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return asciiSrc;
}

nlohmann::json readResourceJSON(const std::filesystem::path &path)
{
    return nlohmann::json::parse( readResourceASCII( path ));
}
#if 0
void combineJSON(std::filesystem::path root, std::filesystem::path gltfIn, std::filesystem::path gltfOut)
{
    auto images_p      = root / "images";
    auto materials_p   = root / "materials";
    auto textures_p    = root / "textures";
    auto bufferViews_p = root / "bufferViews";

    std::ifstream in( gltfIn );

    if( !in )
        exit(1);

    uGLTF::GLTFModel M_in;
    M_in.load(in);
    auto & M = M_in;


    if( std::filesystem::exists(materials_p) )
    {
        size_t i=0;
        for(auto & p : std::filesystem::directory_iterator(materials_p)  )
        {
            std::filesystem::path P(p);
            if( is_number(P.stem()) != -1 )
            {
                std::cout << fmt::format("Replacing Material {}", i) << std::endl;
                auto J = readResourceJSON(P);
                auto m = J.get<uGLTF::Material>();
                M.materials[i] = m;
            }
            i++;
        }
    }
    if( std::filesystem::exists(textures_p) )
    {
        for(auto & p : std::filesystem::directory_iterator(textures_p)  )
        {
            std::filesystem::path P(p);
            if( is_number(P.stem()) != -1  )
            {
                std::cout << fmt::format("Replacing Texture") << std::endl;
                auto J = readResourceJSON(P);
                auto M = J.get<uGLTF::Material>();
                std::cout << "Found: " << is_number(P.stem()) << std::endl;
            };
        }
    }
    if( std::filesystem::exists(images_p) )
    {
        for(auto & p : std::filesystem::directory_iterator(images_p)  )
        {
            std::filesystem::path P(p);

            std::ifstream stream(P, std::ios::in | std::ios::binary);
            std::vector<uint8_t> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

            int x=0;
            int y=0;
            int comp=0;
            auto img_d = stbi_load_from_memory( static_cast<const stbi_uc*>(contents.data()), static_cast<int>(contents.size()), &x, &y, &comp,0);
            std::cout << fmt::format("Found Image: {}  Dim: {}x{}", P.string(), x,y) << std::endl;
            stbi_image_free(img_d);
        }
    }


    std::ofstream out(gltfOut);
    M.writeGLB( out );
    out.close();
}
#endif
int extractJSON(std::filesystem::path outputFolder, std::filesystem::path gltfIn)
{
    std::ifstream in( gltfIn );
    ugltf::GLTFModel2 M_in;

    M_in.load(in);

    if( std::filesystem::exists(outputFolder) )
    {
        if( std::filesystem::is_directory(outputFolder) )
        {

        }
        else
        {
            if( !std::filesystem::is_empty(outputFolder) )
            {
                std::cout << fmt::format("Director, {}, is not empty", outputFolder.string() ) << std::endl;
                exit (1);
            }
        }
    }
    else
    {
        std::filesystem::create_directory( outputFolder );
    }
    std::filesystem::path dir = outputFolder;


    std::set<uint32_t> writtenBufferViews;
    {
        size_t i=0;
        for(auto & m : M_in.images)
        {
            writtenBufferViews.insert(m.bufferView);
            std::filesystem::create_directory( dir / "images" );

            auto mf = dir / "images" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            out.close();
            {
                auto & bv = M_in.bufferViews[m.bufferView];

                std::string ext;
                if( m.mimeType == "image/jpeg")
                {
                    ext = std::to_string(m.bufferView) + ".jpeg";
                }
                else
                {
                    ext = std::to_string(m.bufferView) + ".png";
                }

                std::filesystem::create_directory( dir / "bufferViews" );
                auto mf2 = dir / "bufferViews" / ext;

                std::fstream out2(mf2, std::ios::out | std::ios::binary);
                out2.write( reinterpret_cast<char const*>(bv.data.data()), bv.data.size() );
                out2.close();
            }
            i++;
        }
    }
    {
        size_t i=0;

        for(auto & m : M_in.bufferViews )
        {
            if( writtenBufferViews.count(i) == 0 )
            {
                std::filesystem::create_directory( dir / "bufferViews" );
                //auto mf = dir / "bufferViews" / std::to_string(i);
                //std::ofstream out(mf);
                //out << ugltf::json(m).dump(4);

                {
                    auto mf2 = dir / "bufferViews" / fmt::format("{}.bin", i);
                    std::fstream out2(mf2, std::ios::out | std::ios::binary);
                    out2.write( reinterpret_cast<char const*>( m.data.data()), m.data.size());
                    out2.close();
                    writtenBufferViews.insert(i);
                }
            }
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.accessors)
        {
            std::filesystem::create_directory( dir / "accessors" );
            auto mf = dir / "accessors" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.materials )
        {
            std::filesystem::create_directory( dir / "materials" );
            auto mf = dir / "materials" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.textures)
        {
            std::filesystem::create_directory( dir / "textures" );
            auto mf = dir / "textures" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.meshes)
        {
            std::filesystem::create_directory( dir / "meshes" );
            auto mf = dir / "meshes" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.samplers)
        {
            std::filesystem::create_directory( dir / "samplers" );
            auto mf = dir / "samplers" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.animations)
        {
            std::filesystem::create_directory( dir / "animations" );
            auto mf = dir / "animations" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.nodes)
        {
            std::filesystem::create_directory( dir / "nodes" );
            auto mf = dir / "nodes" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        size_t i=0;
        for(auto & m : M_in.scenes)
        {
            std::filesystem::create_directory( dir / "scenes" );
            auto mf = dir / "scenes" / std::to_string(i);
            std::ofstream out(mf);
            out << ugltf::json(m).dump(4);
            i++;
        }
    }
    {
        auto mf = dir / "asset";
        std::ofstream out(mf);
        out << ugltf::json(M_in.asset).dump(4);
    }
    return 0;
}

int main(int argc, char ** argv)
{
 //   int width = 0;
    std::string name;
    std::string fileName;
    bool show_help = false;
    std::string combine;
    std::string output;
    std::string input;

    auto cli
        = lyra::help(show_help).description(
                    "Extract all materials/textures")
        | lyra::opt( output, "output")
            ["--output"]
            ("Name of output folder" )
        | lyra::opt( input, "input")
            ["--input"]
            ("Input GLTF file" )
        | lyra::opt( combine, "folder")
            ["--combine"]
            ("Combine json files" );
        //| lyra::arg( fileName, "input GLTF/GLB file" )
        //    ("Which GLTF asset to read");


    auto result = cli.parse( { argc, argv } );
    if ( !result )
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;

        exit(1);
    }
    if (show_help || !result)
    {
        std::cout << cli << "\n";
        return 0;
    }


    if( combine != "" && output != "" )
    {
      //  combineJSON( combine, std::filesystem::path(fileName) , std::filesystem::path(output));
      //  return 0;
    }
    else
    {
        extractJSON( std::filesystem::path(output), std::filesystem::path(input) );
    }

/*
    std::ifstream in( fileName );

    if( in )
    {
        uGLTF::GLTFModel M_in;
        M_in.load(in);
        auto & M = M_in;

        auto & J = M._json;

        std::filesystem::path dir(fileName + ".extract");
        std::filesystem::create_directory( dir );

        {
            size_t i=0;
            for(auto & m : J.at("bufferViews"))
            {
                std::filesystem::create_directory( dir / "bufferViews" );
                auto mf = dir / "bufferViews" / std::to_string(i);
                std::ofstream out(mf);
                out << m.dump(4);
                i++;
            }
        }
        {
            size_t i=0;
            for(auto & m : J.at("accessors"))
            {
                std::filesystem::create_directory( dir / "accessors" );
                auto mf = dir / "accessors" / std::to_string(i);
                std::ofstream out(mf);
                out << m.dump(4);
                i++;
            }
        }
        {
            size_t i=0;
            for(auto & m : J.at("materials"))
            {
                std::filesystem::create_directory( dir / "materials" );
                auto mf = dir / "materials" / std::to_string(i);
                std::ofstream out(mf);
                out << m.dump(4);
                i++;
            }
        }
        {
            size_t i=0;
            for(auto & m : J.at("textures"))
            {
                std::filesystem::create_directory( dir / "textures" );
                auto mf = dir / "textures" / std::to_string(i);
                std::ofstream out(mf);
                out << m.dump(4);
                i++;
            }
        }
        {
            size_t i=0;
            for(auto & m : J.at("meshes"))
            {
                std::filesystem::create_directory( dir / "meshes" );
                auto mf = dir / "meshes" / std::to_string(i);
                std::ofstream out(mf);
                out << m.dump(4);
                i++;
            }
        }
        {
            size_t i=0;
            for(auto & m : J.at("images"))
            {
                std::filesystem::create_directory( dir / "images" );

                {
                    auto mf = dir / "images" / std::to_string(i);
                    std::ofstream out(mf);
                    out << m.dump(4);
                }
                std::string ext;
                if( M.images.at(i).mimeType == "image/jpeg")
                {
                    ext = ".jpeg";
                }
                else
                {
                    ext = ".png";
                }
                auto mf = dir / "images" / (std::to_string(i) + ext);
                std::fstream out(mf, std::ios::out | std::ios::binary);

                if( M.images.at(i).m_imageData.size() )
                {
                    out.write((char*)M.images.at(i).m_imageData.data(), M.images.at(i).m_imageData.size());
                    out.close();
                }
                else
                {
                    auto & bv= M.images.at(i).getBufferView();
                    std::cout << bv.byteLength << std::endl;
                    out.write((char*)bv.data(), bv.byteLength);
                    out.close();
                }

                //auto endTime = std::chrono::high_resolution_clock::now();
                //
                //out << m.dump(4);
                i++;
            }
        }
        //std::cout << M._json.dump(4) << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "File does not exist:  " << fileName << std::endl;
        return 1;
    }
*/

}
