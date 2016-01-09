















G4VIStore* B02ImportanceDetectorConstruction::CreateImportanceStore()
{
	
	
	G4cout << " B02ImportanceDetectorConstruction:: Creating Importance Store " 
	<< G4endl;
	if (!fPVolumeStore.Size())
	{
		G4Exception("B02ImportanceDetectorConstruction::CreateImportanceStore"
		,"exampleB02_0001",RunMustBeAborted
		,"no physical volumes created yet!");
	}
	
	// creating and filling the importance store
	
	//  G4IStore *istore = new G4IStore(*fWorldVolume);
	
	G4IStore *istore = G4IStore::GetInstance(GetName());
	
	G4GeometryCell gWorldVolumeCell(GetWorldVolumeAddress(), 0);
	
	G4double imp =1;
	
	istore->AddImportanceGeometryCell(1, gWorldVolumeCell);
	
	// set importance values and create scorers 
	G4int cell(1);
	for (cell=1; cell<=18; cell++) {
		G4GeometryCell gCell = GetGeometryCell(cell);
		G4cout << " adding cell: " << cell 
		<< " replica: " << gCell.GetReplicaNumber() 
		<< " name: " << gCell.GetPhysicalVolume().GetName() << G4endl;
		imp = std::pow(2.0,cell-1);
		
		G4cout << "Going to assign importance: " << imp << ", to volume: " 
		<< gCell.GetPhysicalVolume().GetName() << G4endl;
		//x    aIstore.AddImportanceGeometryCell(imp, gCell);
		istore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), cell);
	}
	
	// creating the geometry cell and add both to the store
	//  G4GeometryCell gCell = GetGeometryCell(18);
	
	// create importance geometry cell pair for the "rest"cell
	// with the same importance as the last concrete cell 
	G4GeometryCell gCell = GetGeometryCell(19);
	//  G4double imp = std::pow(2.0,18); 
	imp = std::pow(2.0,17); 
	istore->AddImportanceGeometryCell(imp, gCell.GetPhysicalVolume(), 19);
	
	return istore;
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VWeightWindowStore *B02ImportanceDetectorConstruction::
CreateWeightWindowStore()
{
	G4cout << " B02ImportanceDetectorConstruction:: Creating Importance Store " 
	<< G4endl;
	if (!fPVolumeStore.Size())
	{
		G4Exception("B02ImportanceDetectorConstruction::CreateWeightWindowStore"
		,"exampleB02_0002",RunMustBeAborted
		,"no physical volumes created yet!");
	}
	
	// creating and filling the importance store
	
	//  G4IStore *istore = new G4IStore(*fWorldVolume);
	
	G4WeightWindowStore *wwstore = G4WeightWindowStore::GetInstance(GetName());
	
	
	// create one energy region covering the energies of the problem
	//
	std::set<G4double, std::less<G4double> > enBounds;
	enBounds.insert(1 * GeV);
	wwstore->SetGeneralUpperEnergyBounds(enBounds);
	
	G4int n = 0;
	G4double lowerWeight =1;
	std::vector<G4double> lowerWeights;
	
	lowerWeights.push_back(1);
	G4GeometryCell gWorldCell(GetWorldVolumeAddress(),0);
	wwstore->AddLowerWeights(gWorldCell, lowerWeights);
	
	G4int cell(1);
	for (cell=1; cell<=18; cell++) {
		G4GeometryCell gCell = GetGeometryCell(cell);
		G4cout << " adding cell: " << cell 
		<< " replica: " << gCell.GetReplicaNumber() 
		<< " name: " << gCell.GetPhysicalVolume().GetName() << G4endl;
		
		lowerWeight = 1./std::pow(2., n++);
		G4cout << "Going to assign lower weight: " << lowerWeight 
		<< ", to volume: " 
		<< gCell.GetPhysicalVolume().GetName() << G4endl;
		lowerWeights.clear();
		lowerWeights.push_back(lowerWeight);
		wwstore->AddLowerWeights(gCell, lowerWeights);
	}
	
	// the remaining part pf the geometry (rest) gets the same
	// lower weight bound  as the last conrete cell
	//
	
	// create importance geometry cell pair for the "rest"cell
	// with the same importance as the last concrete cell 
	G4GeometryCell gCell = GetGeometryCell(19);
	wwstore->AddLowerWeights(gCell,  lowerWeights);
	
	
	return wwstore;
	
}