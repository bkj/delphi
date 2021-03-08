#include <iostream>
#include <served/served.hpp>
#include <nlohmann/json.hpp>
//#include "dbg"
#include "AnalysisGraph.hpp"
#include <string>
#include <assert.h>
#include <sqlite3.h>
#include <range/v3/all.hpp>
#include "DatabaseHelper.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.



using namespace std;
using json = nlohmann::json;



void testcase_Database_Create(Database* sqlite3DB)
{
    std::cout << "\n======================= Test testcase_Database_Create =======================\n" << std::endl;
    
    sqlite3DB->Database_Create();
}


void testcase_Database_Read_ColumnText(Database* sqlite3DB)
{
    std::cout << "\n======================= Test testcase_Database_Read_ColumnText =======================\n" << std::endl;
    
    string query = "SELECT adjective from gradableAdjectiveData;";
    //string query = "select Unit from indicator;";
    vector<string> matches = sqlite3DB->Database_Read_ColumnText(query);
    cout << "Read vector size: " << matches.size() << endl;
}


void testcase_Database_Insert(Database* sqlite3DB)
{
    std::cout << "\n======================= Test testcase_Database_Insert =======================\n" << std::endl;
    
    string query = "INSERT INTO concept_to_indicator_mapping ('Concept', 'Source', 'Indicator', 'Score') VALUES ('wm/concept/time/temporal/crop_season', 'delphi_db_inds_TEST', 'TEST', 0.49010614);";
    sqlite3DB->Database_Insert(query);
    
    query = "SELECT Source from concept_to_indicator_mapping WHERE Indicator = 'TEST';";
    vector<string> matches = sqlite3DB->Database_Read_ColumnText(query);
    cout << "Read vector size: " << matches.size() << endl;
    if(matches.size()) cout << "Read Source column value: " << matches[0] << endl;
}

void testcase_Database_Update(Database* sqlite3DB)
{
    std::cout << "\n======================= Test testcase_Database_Update =======================\n" << std::endl;
    
    sqlite3DB->Database_Update("concept_to_indicator_mapping", "Source", "delphi_db_inds_TEST_update_2", "Indicator", "TEST");
        void Database_Update(string table_name, string column_name, string value, string where_column_name, string where_value);

    string query = "SELECT Source from concept_to_indicator_mapping WHERE Indicator = 'TEST';";
    vector<string> matches = sqlite3DB->Database_Read_ColumnText(query);
    cout << "Read vector size: " << matches.size() << endl;
    if(matches.size()) cout << "Read Source column value: " << matches[0] << endl;
}




//class DelphiModel(){
//    public:
//        string __tablename__;
//        string id;
//        string model;
//        //id = db.Column(db.String, primary_key=True)
//        //model = db.Column(db.String)
//
//        DelphiModel(string _id, string _model){
//            this->__tablename__ = "delphimodel";
//            this->id = _id;
//            this->model = _model;
//        }        
//};




class CauseMosAsyncExperimentResult{
    /* Placeholder docstring for class CauseMosAsyncExperimentResult. */
    public:
        string id;
        string status;
        string experimentType;
        json results;


        CauseMosAsyncExperimentResult(){
            //id = db.Column(
            //    db.String,
            //    db.ForeignKey("experimentresult.id"),
            //    primary_key=True,
            //    default=str(uuid4()),
            //)
            //status = db.Column(db.String, nullable=True)
            //experimentType = db.Column(db.String, nullable=True)
            //results = db.Column(JsonEncodedDict, nullable=True)
        }
};


int main(int argc, const char *argv[])
{
    Database* sqlite3DB = new Database();


    //================= Testsuite =============================================
    // Run Tests:
    //testcase_Database_Create(sqlite3DB); 
    
    // working
    //testcase_Database_Read_ColumnText(sqlite3DB); 
    //testcase_Database_Insert(sqlite3DB); 
    //testcase_Database_Update(sqlite3DB); 
    // drop test rows from delphi.db

    //======================App=================================================    
    served::multiplexer mux;


    mux.handle("/delphi/create-model")
        .post([&sqlite3DB](served::response & response, const served::request & req) {
            nlohmann::json json_data = nlohmann::json::parse(req.body());
            size_t res = 1000;
            if(getenv("CI")) 
                res = 5;
            else if (getenv("DELPHI_N_SAMPLES")) {
                res = (size_t)stoul(getenv("DELPHI_N_SAMPLES"));
            }

            AnalysisGraph G;
            G.set_res(res);
            G.from_causemos_json_dict(json_data);

            sqlite3DB->Database_InsertInto_delphimodel(json_data["id"], G.serialize_to_json_string(false));

            //res <<  nlohmann::json::parse(G.generate_create_model_response());
            response << json_data;
        });



// 2nd createCausemosExperiment
    mux.handle("/delphi/models/{modelID}/experiments")
        .post([&sqlite3DB](served::response & res, const served::request & req) {
            auto request_body = nlohmann::json::parse(req.body());
            string modelID = req.params["modelID"];

            string experiment_type = request_body["experimentType"]; //??
            boost::uuids::uuid uuid = boost::uuids::random_generator()(); // todo: error: no member named 'uuids' in namespace 'boost'
            string experiment_id = to_string(uuid);

            cout << "Before Insert" << endl;
            //string experiment_id = "123";
            // TODO:  add not update check
            sqlite3DB->Database_InsertInto_causemosasyncexperimentresult(experiment_id, "in progress", experiment_type, "");
            cout << "After Insert" << endl;


        });





    std::cout << "Try this example with:" << std::endl;
    std::cout << "curl -X POST \"http://localhost:8123/delphi/create-model\" -d @delphi/tests/data/delphi/causemos_create-model.json --header \"Content-Type: application/json\" " << std::endl;
    std::cout << "curl -X POST \"http://localhost:8123/delphi/models/modelID/experiments\" -d @test.json --header \"Content-Type: application/json\" " << std::endl;

    served::net::server server("127.0.0.1", "8123", mux);
    server.run(10);

    return (EXIT_SUCCESS);
}

/*


// 4th check datatypes

void runProjectionExperiment(const served::request & request, string modelID, string experiment_id, AnalysisGraph G, bool trained){
    auto request_body = nlohmann::json::parse(request.body());


    string startTime = request_body["experimentParam"]["startTime"];
    string endTime = request_body["experimentParam"]["endTime"];
    int numTimesteps = request_body["experimentParam"]["numTimesteps"];

    FormattedProjectionResult causemos_experiment_result = G.run_causemos_projection_experiment(
        request_body // todo: ??????
    );

    DelphiModel model;
    if(not trained){
        //model = DelphiModel(modelID, G.serialize_to_json_string(false));

        //db.session.merge(model)
        //db.session.commit()

        string id = "123";
        string model = "TEST";
        sqlite3DB->Database_InsertInto_delphimodel(modelID, G.serialize_to_json_string(false));
    }

    // created causemosasyncexperimentresult class
    //result = CauseMosAsyncExperimentResult.query.filter_by(
    //    id=experiment_id
    //).first()
    vector<string> matches = sqlite3DB->Database_Read_ColumnText_Wrapper("causemosasyncexperimentresult", ,"id", experiment_id);


    vector<vector<vector<double>>> formattedProjectionTimestepSample;
    for (const auto & [ key, value ] : causemos_experiment_result) {
        formattedProjectionTimestepSample.push_back(value);
    }
    if(formattedProjectionTimestepSample[0].size() < numTimesteps)
        result.status = "failed";
    else{
        result.status = "completed";

        // todo !!!!!1
        double timesteps_nparr = np.round(
            np.linspace(startTime, endTime, numTimesteps)l
        )

        // The calculation of the 95% confidence interval about the median is
        // taken from:
        // https://www.ucl.ac.uk/child-health/short-courses-events/ \
        //     about-statistical-courses/research-methods-and-statistics/chapter-8-content-8
        int n = G.get_res();

        int lower_rank = (int)((n - 1.96 * sqrt(n)) / 2);
        int upper_rank = (int)((2 + n + 1.96 * sqrt(n)) / 2);

        lower_rank = lower_rank < 0 ? 0 : lower_rank;
        upper_rank = upper_rank >= n ? n - 1 : upper_rank;
        unordered_map<string, vector<string>> res_data; // vector<string> datatype ???
        res_data["data"] = {};
        
        result.results = res_data; 

        for (const auto & [ conceptname, timestamp_sample_matrix ] : causemos_experiment_result) {
            json data_dict;
            data_dict["concept"] = conceptname;
            data_dict["values"] = vector<unordered_map<string, double>>{};
            unordered_map<string, vector<double>> m { {"upper", vector<double>{}}, {"lower", vector<double>{}}};
            data_dict["confidenceInterval"] = m;
            for(int i = 0; i < timestamp_sample_matrix.size(); i++){
                vector<double> time_step = timestamp_sample_matrix[i];
                sort(time_step.begin(), time_step.end());
                int l = time_step.size() / 2;
                double median_value = 0;
                if(time_step.size())
                    median_value = time_step.size() % 2? time_step[l]: (time_step[l] + time_step[l - 1]) / 2;

                double lower_limit = time_step[lower_rank];
                double upper_limit = time_step[upper_rank];

                unordered_map<string, double> value_dict = {
                    {"timestamp", timesteps_nparr[i]},
                    {"value", median_value}
                };

                data_dict["values"].push_back(value_dict);
                value_dict["value"] = lower_limit;
                data_dict["confidenceInterval"]["lower"].push_back(value_dict);

                value_dict["value"] = upper_limit;
                data_dict["confidenceInterval"]["upper"].push_back(value_dict);
            }
            result.results["data"].push_back(data_dict); 
        }
    }

    // result = CauseMosAsyncExperimentResult
    // db.session.merge: update in already, else insert

    //db.session.merge(result)
    //db.session.commit()

}

*/

// 3rd runExperiment
void runExperiment(const served::request & request, string modelID, string experiment_id, bool trained){
    auto request_body = nlohmann::json::parse(request.body());
    string experiment_type = request_body["experimentType"]; 


    ////query_result = DelphiModel.query.filter_by(id=modelID).first()

    if(query_result.empty()){
        // Model ID not in database. Should be an incorrect model ID
        result = CauseMosAsyncExperimentResult.query.filter_by(
            id=experiment_id
        ).first()
        result.status = "failed"
        db.session.merge(result)
        db.session.commit()
        return
    }

    model = query_result.model
    trained = request_body["trained"];
    AnalysisGraph G;
    G = G.deserialize_from_json_string(model, false);

    if(experiment_type == "PROJECTION")
        runProjectionExperiment(request, modelID, experiment_id, G, trained);
    else if(experiment_type == "GOAL_OPTIMIZATION")
        // Not yet implemented
    else if( experiment_type == "SENSITIVITY_ANALYSIS")
        // Not yet implemented
    else if( experiment_type == "MODEL_VALIDATION")
        // Not yet implemented
    else if( experiment_type == "BACKCASTING")
        // Not yet implemented
    else
        // Unknown experiment type
}















/*
Todo
Remove done from api.py

*/

 