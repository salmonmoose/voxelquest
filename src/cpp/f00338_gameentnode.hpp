


/////////////////////////


//   IMPORTANT: SPECIFY ZERO VECTOR FOR BASE ROTATION
//   (TWO VECTORS TOTAL FOR ROTATION AND ORIENTATION)


/////////////////////////


class GameEntNode {

private:

public:
	
	float rotThe; // vertical rotation
	float rotPhi; // horizontal rotation
	float rotRho;
	
	float material;
	float boneLengthHalf;
	
	FIVector4* readTBN;
	FIVector4* writeTBN;
	
	FIVector4 tbnBase[3];
	FIVector4 tbnRotA[3];
	FIVector4 tbnRotB[3];
	FIVector4 tbnRotC[3];
	FIVector4 tbnTrans[3];
	
	
	FIVector4 tbnRadInMeters0;
	FIVector4 tbnRadInMeters1;

	FIVector4 tbnRadScale0;
	FIVector4 tbnRadScale1;

	// transformed points
	FIVector4 orgTrans[3]; // start, middle, end
	
	

	int nodeName;
	

	GameEntNode* parent;
	std::vector<GameEntNode*> children;

	GameEntNode(
		GameEntNode* _parent,
		int _nodeName,
		float _boneLength,
		float _tanLengthInMeters,
		float _bitLengthInMeters,
		float _norLengthInMeters,
		
		float _tanX, float _tanY, float _tanZ,
		float _bitX, float _bitY, float _bitZ,
		float _norX, float _norY, float _norZ
		
		
	) {
		material = 8.0;
		
		parent = _parent;
		nodeName = _nodeName;
		
		rotThe = 0.0f;
		rotPhi = 0.0f;
		rotRho = 0.0f;

		boneLengthHalf = _boneLength*0.5f;

		tbnRadInMeters0.setFXYZ(
			_tanLengthInMeters,
			_bitLengthInMeters,
			_norLengthInMeters
		);
		tbnRadInMeters1.setFXYZRef(&tbnRadInMeters0);
		tbnRadScale0.setFXYZ(1.0f,1.0f,1.0f);
		tbnRadScale1.setFXYZ(1.0f,1.0f,1.0f);
		
		
		(tbnBase[0]).setFXYZ(_tanX,_tanY,_tanZ);
		(tbnBase[1]).setFXYZ(_bitX,_bitY,_bitZ);
		(tbnBase[2]).setFXYZ(_norX,_norY,_norZ);
		(tbnBase[0]).normalize();
		(tbnBase[1]).normalize();
		(tbnBase[2]).normalize();
		
		// FIVector4::cross(&(tbnBase[2]),&(tbnBase[0]),&(tbnBase[1]));
		// (tbnBase[2]).normalize();
	}
	
	GameEntNode* addChild(
		int _nodeName,
		float _boneLength,
		float _tanLengthInMeters,
		float _bitLengthInMeters,
		float _norLengthInMeters,
		
		float _tanX, float _tanY, float _tanZ,
		float _bitX, float _bitY, float _bitZ,
		float _norX, float _norY, float _norZ
	) {
		
		//if (_nodeName >= E_BONE_C_END) {
		//	cout << "nodeName: " << _nodeName << "\n";
		//}
		
		children.push_back(
			new GameEntNode(
				this,
				
				_nodeName,
				_boneLength,
				_tanLengthInMeters,
				_bitLengthInMeters,
				_norLengthInMeters,
				
				_tanX, _tanY, _tanZ,
				_bitX, _bitY, _bitZ,
				_norX, _norY, _norZ
			)
		);
		
		return children.back();
	}
	
	GameEntNode* getNode(int _nodeName) {
		int i;
		
		// cout << "getNode("<< _nodeName << "): " << nodeName << "\n";
		
		
		if (nodeName == _nodeName) {
			return this;
		}
		
		for (i = 0; i < children.size(); i++) {
			
			if (children[i]->getNode(_nodeName) != NULL) {
				return children[i]->getNode(_nodeName);
			}
		}
		
		return NULL;
	}
	
	//void rotate(FIVector4 axis, )
	
	void doTransform(
		Singleton* singleton
	) {
		
		int i;
		int j;
		int popCount = 0;
		
		//cout << "doTransform: " << boneStrings[nodeName] << "\n"; 
		
		
		// void doRotation(
		// 	FIVector4 *output,
		// 	FIVector4 *input,
		// 	FIVector4 *axis,
		// 	float angle
		// )
		
		
		// start
		if (parent == NULL) {
			orgTrans[0].setFXYZ(0.0f,0.0f,0.0f);
		}
		else {
			orgTrans[0].setFXYZRef(&(parent->orgTrans[2]));
		}
		
		
		
		
		
		
		readTBN = tbnBase;
		writeTBN = tbnRotA;
		
		
		// if (rotPhi != 0.0f) {
		// 	singleton->rotStack.push_back(readTBN[0]);
		// 	singleton->rotStack.back().setFW(rotPhi);
		// 	singleton->transStack.push_back(orgTrans[0]);			
		// 	popCount++;
		// }
		// if (rotThe != 0.0f) {
		// 	singleton->rotStack.push_back(readTBN[1]);
		// 	singleton->rotStack.back().setFW(rotThe);
		// 	singleton->transStack.push_back(orgTrans[0]);
		// 	popCount++;
		// }
		// if (rotRho != 0.0f) {
		// 	singleton->rotStack.push_back(readTBN[2]);
		// 	singleton->rotStack.back().setFW(rotRho);
		// 	singleton->transStack.push_back(orgTrans[0]);
		// 	popCount++;
		// }
		
		
		
		for (i = 0; i < singleton->rotStack.size(); i++) {
			
			axisRotationInstance.doRotationTBN(
				writeTBN, // write
				readTBN, // read
				&(singleton->rotStack[i]),
				&(singleton->transStack[i]),
				&(orgTrans[0])
			);
			
			if ((i%2) == 0) {
				readTBN = tbnRotA;
				writeTBN = tbnRotB;
			}
			else {
				readTBN = tbnRotB;
				writeTBN = tbnRotA;
			}
			
		}
		
		
		if (rotPhi != 0.0f) {
			singleton->rotStack.push_back(readTBN[0]);
			singleton->rotStack.back().setFW(rotPhi);
			singleton->transStack.push_back(orgTrans[0]);
			
			axisRotationInstance.doRotationTBN(
				writeTBN, // write
				readTBN, // read
				&(singleton->rotStack.back()),
				&(singleton->transStack.back()),
				&(orgTrans[0])
			);
			if ((i%2) == 0) {
				readTBN = tbnRotA;
				writeTBN = tbnRotB;
			}
			else {
				readTBN = tbnRotB;
				writeTBN = tbnRotA;
			}
			i++;
			
			popCount++;
		}
		if (rotThe != 0.0f) {
			singleton->rotStack.push_back(readTBN[1]);
			singleton->rotStack.back().setFW(rotThe);
			singleton->transStack.push_back(orgTrans[0]);
			
			axisRotationInstance.doRotationTBN(
				writeTBN, // write
				readTBN, // read
				&(singleton->rotStack.back()),
				&(singleton->transStack.back()),
				&(orgTrans[0])
			);
			if ((i%2) == 0) {
				readTBN = tbnRotA;
				writeTBN = tbnRotB;
			}
			else {
				readTBN = tbnRotB;
				writeTBN = tbnRotA;
			}
			i++;
			
			popCount++;
		}
		if (rotRho != 0.0f) {
			singleton->rotStack.push_back(readTBN[2]);
			singleton->rotStack.back().setFW(rotRho);
			singleton->transStack.push_back(orgTrans[0]);
			
			axisRotationInstance.doRotationTBN(
				writeTBN, // write
				readTBN, // read
				&(singleton->rotStack.back()),
				&(singleton->transStack.back()),
				&(orgTrans[0])
			);
			if ((i%2) == 0) {
				readTBN = tbnRotA;
				writeTBN = tbnRotB;
			}
			else {
				readTBN = tbnRotB;
				writeTBN = tbnRotA;
			}
			i++;
			
			popCount++;
		}
		
		
		
		for (j = 0; j < 3; j++) {
			tbnRotC[j].setFXYZRef(&( readTBN[j] ));
		}
		
		
		// middle
		orgTrans[1].setFXYZRef(&(tbnRotC[0]));
		orgTrans[1].multXYZ(boneLengthHalf);
		orgTrans[1].addXYZRef(&(orgTrans[0]));
		
		// end
		orgTrans[2].setFXYZRef(&(tbnRotC[0]));
		orgTrans[2].multXYZ(boneLengthHalf*2.0f);
		orgTrans[2].addXYZRef(&(orgTrans[0]));
		
		for (i = 0; i < 3; i++) {
			(tbnTrans[i]).setFXYZRef(&(tbnRotC[i]));
			(tbnTrans[i]).multXYZ(tbnRadInMeters0[i]*tbnRadScale0[i]);
			(tbnTrans[i]).addXYZRef(&(orgTrans[1]));
		}
		
		
		
		for (i = 0; i < children.size(); i++) {
			children[i]->doTransform(singleton);
		}
		
		
		for (i = 0; i < popCount; i++) {
			
			singleton->transStack.pop_back();
			singleton->rotStack.pop_back();
		}
		
	}
	
};
