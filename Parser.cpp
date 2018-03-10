#include "Parser.h"

// Parses xml data
void Parser::scene_parser(char* filename, Scene* scene) {
    // Open the file
    pugi::xml_document xml;
    xml.load_file(filename);

    // Set up the world
    pugi::xml_node world = xml.child("World");
    scene->ambient.set_RGB(world.child("ambient").child("color").attribute("r").as_double(),
                           world.child("ambient").child("color").attribute("g").as_double(),
                           world.child("ambient").child("color").attribute("b").as_double());

    // The camera
    pugi::xml_node camera = xml.child("Camera");
    scene->camera = new Camera(Vector3d(0, 0, 0), Matrix3d::Identity(), 60);
    scene->camera->horitzontal_fov = camera.child("fov").text().as_int()*M_PI/180;

    // The materials
    pugi::xml_node materials = xml.child("Materials");
    scene->materials.resize(materials.select_nodes("material").size());
    for(pugi::xml_node material = materials.child("material"); material; material = material.next_sibling("material")) {
        int id = material.attribute("id").as_int();
        pugi::xml_node shader = material.child("shader");
        pugi::xml_node attributes = shader.child("attributes");
        std::string brdf = shader.attribute("brdf").as_string();
        if(brdf=="diffuse") {
            scene->materials[id] = new Diffuse(Color4dRGB(shader.child("color").attribute("r").as_double(),
                                                          shader.child("color").attribute("g").as_double(),
                                                          shader.child("color").attribute("b").as_double()));
        } else if(brdf=="GGX") {
            scene->materials[id] = new GGX(shader.attribute("roughness").as_double(), Color4dRGB(
                                               shader.child("color").attribute("r").as_double(),
                                               shader.child("color").attribute("g").as_double(),
                                               shader.child("color").attribute("b").as_double()),
                                           shader.attribute("ior").as_double());
        } else if(brdf=="Reflective") {
            scene->materials[id] = new Reflective();
        } else if(brdf=="Refractive") {
            scene->materials[id] = new Refractive(shader.attribute("ior").as_double());
        } else if(brdf=="emissive") {
            scene->materials[id] = new Emissive(Color4dRGB(shader.child("color").attribute("r").as_double(),
                                                           shader.child("color").attribute("g").as_double(),
                                                           shader.child("color").attribute("b").as_double()));
        }
    }

    std::cout<<scene->materials.size()<<" number of materials"<<std::endl;

    // Lights
    pugi::xml_node lights = xml.child("Lights");
    for(pugi::xml_node light = lights.child("light"); light; light = light.next_sibling("light")) {
        int id = light.attribute("id").as_int();
        std::string type = light.attribute("type").as_string();
        pugi::xml_node color_ = light.child("color");
        Color4dRGB color = Color4dRGB(color_.attribute("r").as_double(),
                                      color_.attribute("g").as_double(),
                                      color_.attribute("b").as_double());

        if(type=="point") {
            scene->lights.push_back(new PointLight(Vector3d(light.child("position").attribute("x").as_double(),
                                                            light.child("position").attribute("y").as_double(),
                                                            light.child("position").attribute("z").as_double()),
                                                   color,
                                                   light.attribute("power").as_double(),
                                                   light.attribute("radius").as_double(),
                                                   light.attribute("softShadows").as_bool()));
        } else if(type=="area") {
            scene->lights.push_back(new AreaLight(
                Vector3d(light.child("a").attribute("x").as_double(),
                         light.child("a").attribute("y").as_double(),
                         light.child("a").attribute("z").as_double()),
                Vector3d(light.child("b").attribute("x").as_double(),
                         light.child("b").attribute("y").as_double(),
                         light.child("b").attribute("z").as_double()),
                Vector3d(light.child("c").attribute("x").as_double(),
                         light.child("c").attribute("y").as_double(),
                         light.child("c").attribute("z").as_double()),
                Vector3d(light.child("d").attribute("x").as_double(),
                         light.child("d").attribute("y").as_double(),
                         light.child("d").attribute("z").as_double()),
                color
            ));
        }
    }

    std::map<int, int> prmtv_parents;
    // Objects
    pugi::xml_node objects = xml.child("Objects");
    scene->objects.resize(objects.select_nodes("object").size());
    for(pugi::xml_node object = objects.child("object"); object; object = object.next_sibling("object")) {
        if(object.child("material").attribute("id").as_int()>=scene->materials.size()) {
            throw std::runtime_error("xml description corrupted: specified material for object is not created.");
        }
        int id = object.attribute("id").as_int();
        int material = object.child("material").attribute("id").as_int();
        Object* obj = new Object(id, scene->materials[material]);

        pugi::xml_node primitives = object.child("primitives");
        int prmtv_id;
        for(pugi::xml_node prmtv = primitives.child("id"); prmtv; prmtv = prmtv.next_sibling("id")) {
            prmtv_id = prmtv.text().as_int();
            prmtv_parents.insert(std::pair<int, int>(prmtv_id, id));
            obj->primitives.push_back(prmtv_id);
        }
        obj->direct_sample_lights = object.attribute("directSampleLights").as_bool();
        obj->direct_sample = object.attribute("directSample").as_bool();
        scene->objects[id] = obj;
    }

    // The Primitives
    pugi::xml_node prmtvs = xml.child("Primitives");
    for(pugi::xml_node prmtv = prmtvs.child("primitive"); prmtv; prmtv = prmtv.next_sibling("primitive")) {
        int prmtv_id = prmtv.attribute("id").as_int();
        int parent_id = prmtv_parents[prmtv_id];

        pugi::xml_node mesh = prmtv.child("mesh");
        std::string type = mesh.attribute("type").as_string();
        if(type=="Sphere") {
            scene->primitives.push_back(new Sphere(scene->objects[parent_id], mesh.child("radius").text().as_double(),
                                                   Vector3d(
                                                       mesh.child("position").attribute("x").as_double(),
                                                       mesh.child("position").attribute("y").as_double(),
                                                       mesh.child("position").attribute("z").as_double()
                                                   )));
        } else if(type=="Plane") { } else if(type=="Tri") {
            scene->primitives.push_back(new Tri(scene->objects[parent_id],
                                                Vector3d(
                                                    mesh.child("a").attribute("x").as_double(),
                                                    mesh.child("a").attribute("y").as_double(),
                                                    mesh.child("a").attribute("z").as_double()
                                                ),
                                                Vector3d(
                                                    mesh.child("b").attribute("x").as_double(),
                                                    mesh.child("b").attribute("y").as_double(),
                                                    mesh.child("b").attribute("z").as_double()
                                                ),
                                                Vector3d(
                                                    mesh.child("c").attribute("x").as_double(),
                                                    mesh.child("c").attribute("y").as_double(),
                                                    mesh.child("c").attribute("z").as_double()
                                                )));
        } else if(type=="Quad") {
            scene->primitives.push_back(new Quad(scene->objects[parent_id],
                                                 Vector3d(
                                                     mesh.child("a").attribute("x").as_double(),
                                                     mesh.child("a").attribute("y").as_double(),
                                                     mesh.child("a").attribute("z").as_double()
                                                 ),
                                                 Vector3d(
                                                     mesh.child("b").attribute("x").as_double(),
                                                     mesh.child("b").attribute("y").as_double(),
                                                     mesh.child("b").attribute("z").as_double()
                                                 ),
                                                 Vector3d(
                                                     mesh.child("c").attribute("x").as_double(),
                                                     mesh.child("c").attribute("y").as_double(),
                                                     mesh.child("c").attribute("z").as_double()
                                                 ),
                                                 Vector3d(
                                                     mesh.child("d").attribute("x").as_double(),
                                                     mesh.child("d").attribute("y").as_double(),
                                                     mesh.child("d").attribute("z").as_double()
                                                 )));
        } else {
            throw std::runtime_error("xml description corrupted: specified mesh type is not supported.");
        }
    }
}

//	Parses given configuration file and overrides global variables
void Parser::configuration_parser(char* filename, Scene* conf) {
    // Open the file
    pugi::xml_document config;
    config.load_file(filename);

    // Canvas properties
    pugi::xml_node canvas = config.child("Canvas");
    conf->width = canvas.child("width").text().as_int();
    conf->height = canvas.child("height").text().as_int();

    // Integrator
    pugi::xml_node integrator = config.child("Integrator");
    std::string integrator_name = integrator.child("integrator").text().as_string();
    if(integrator_name=="pathtracer") {
        PathTracer* pathtracer = new PathTracer();
        pathtracer->n_samples = integrator.child("n_samples").text().as_int();
        pathtracer->min_depth = integrator.child("min_depth").text().as_int();
        pathtracer->max_depth = integrator.child("max_depth").text().as_int();
        conf->integrator = pathtracer;
    } else if(integrator_name=="IS_PathTracer") {
        IS_PathTracer* is_pathtracer = new IS_PathTracer();
        is_pathtracer->NUM_THREADS = integrator.child("NUM_THREADS").text().as_int();
        is_pathtracer->n_indirect_samples = integrator.child("n_indirect_samples").text().as_int();
        is_pathtracer->n_primary_direct_samples = integrator.child("n_primary_direct_samples").text().as_int();
        is_pathtracer->n_secondary_direct_samples = integrator.child("n_secondary_direct_samples").text().as_int();
        is_pathtracer->tile_size = integrator.child("tile_size").text().as_int();
        is_pathtracer->min_depth = integrator.child("min_depth").text().as_int();
        is_pathtracer->max_depth = integrator.child("max_depth").text().as_int();
        if(integrator.child("msaa_level").text().as_int()==4)
            is_pathtracer->msaa_level = MSAA_Level::msaa_4x;
        else if(integrator.child("msaa_level").text().as_int()==8)
            is_pathtracer->msaa_level = MSAA_Level::msaa_8x;
        else
            is_pathtracer->msaa_level = MSAA_Level::none;
        conf->integrator = is_pathtracer;
    } else {
        throw std::runtime_error("Invalid integrator.");
    }

    // Halt conditions
    pugi::xml_node haltConditions = config.child("HaltConditions");
    conf->halt = haltConditions.child("halt").text().as_bool();
    conf->time_limit = haltConditions.child("time").text().as_int();
    conf->incremental_save = haltConditions.child("incrementalSave").text().as_bool();

    // Output specifications
    pugi::xml_node outputConf = config.child("Output");
    conf->out_file = outputConf.child("file").text().as_string();

    //if(width<=0||height<=0||sample<=0||Integrator::PathTracer::max_depth<1) {
    //	throw std::runtime_error("Configuration file corrupted: one of the inputs are out of legal range.");
    //}
}

Scene* Parser::parse(int argc, char* argv[]) {
    std::string configXML = "config.xml";
    std::string sceneXML = "scene.xml";
    if(argc<2) { } else if(argc<4) {
        if(std::string(argv[1])=="-s") {
            sceneXML = argv[2];
        } else if(std::string(argv[1])=="-c") {
            configXML = argv[2];
        } else {
            throw std::runtime_error("Invalid arguments.");
        }
    } else if(argc<6) {
        if(std::string(argv[1])=="-s"&&std::string(argv[3])=="-c") {
            sceneXML = argv[2];
            configXML = argv[4];
        } else if(std::string(argv[1])=="-c"&&std::string(argv[3])=="-s") {
            sceneXML = argv[4];
            configXML = argv[2];
        } else {
            throw std::runtime_error("Invalid arguments.");
        }
    } else {
        throw std::runtime_error("Invalid arguments.");
    }
    Scene* scene = new Scene();
    Parser::configuration_parser(&configXML[0], scene);
    Parser::scene_parser(&sceneXML[0], scene);

    return scene;
}
