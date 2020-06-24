###
Parameters def from Peter

https://github.com/ScottishCovidResponse/temporary_data/tree/peter-t-fox/444-add-eera-params-for-staging/temporary/Covid19_EERAModel/fixed-parameters

### Thibaud has completed Irish model
more general model workflow, abstracted from all other models
Kristian is working on Data Abstraction Layer

To enable correlation validation by other models,  a proper Result data structure

### EERA specific data structure

2 Inputs adn 1 Output:  IO.h
 `EERAModel::ModelInputParameters ReadParametersFromFile()`
 `EERAModel::InputObservations ReadObservationsFromFiles();`
 `WriteOutputsToFiles()`
Will have their DataFlow version, a macro switch can select. currently IO interface with local files will be maintained as requested by model author.

example inputs can be found in "test/regression/data/"

My understanding is "parameter.ini" does not needs any paramter from remote data store. While the other "Observations" could be retrieved from remote data store.

Input dataset `Observations` can be several 2D datasets in HDF5 or TABLEs in database
All data types are defined in ``ModelTypes.h
```c++
struct InputObservations {
	std::vector<std::vector<int>> cases;
	std::vector<std::vector<int>> deaths;
	std::vector<std::vector<double>> age_pop;
	std::vector<std::vector<double>> waifw_norm;
	std::vector<std::vector<double>> waifw_home;
	std::vector<std::vector<double>> waifw_sdist;
	std::vector<std::vector<double>> cfr_byage;
	std::vector<std::vector<double> > pf_pop;
};
// each field map to/READ FROM a CSV files
//  this struct may be split into 2
```
Currently, `Observations` are read from local csv files in a model root_dir
`std::string(ROOT_DIR)+"/data/waifw_home.csv"`

### Output is csv file
mapped to HDF5 dataset (Table of DataBase)
`outvec` at the end of `runModel()`