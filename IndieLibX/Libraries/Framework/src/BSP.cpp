/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\BSP.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/BSP.h>

#undef LOG_FNLN
#undef LOG_PRINT

void BTri::setup(){
    D3DXFROMWINEVECTOR3 v1=v[1] - v[0];
    D3DXFROMWINEVECTOR3 v2=v[2] - v[0];
    D3DXFROMWINEVECTOR3 vO;
    D3DXFROMWINEVec3Cross(&vO, &v1, &v2);
	D3DXFROMWINEVec3Normalize(&normal, &vO);
	offset = -D3DXFROMWINEVec3Dot(&v[0], &normal);

    v1=v[0] - v[2];
	D3DXFROMWINEVec3Cross(&edgeNormals[0], &normal, &v1);
    v1=v[1] - v[0];
	D3DXFROMWINEVec3Cross(&edgeNormals[1], &normal, &v1);
    v1=v[2] - v[1];
	D3DXFROMWINEVec3Cross(&edgeNormals[2], &normal, &v1);

	edgeOffsets[0] = D3DXFROMWINEVec3Dot(&edgeNormals[0], &v[0]);
	edgeOffsets[1] = D3DXFROMWINEVec3Dot(&edgeNormals[1], &v[1]);
	edgeOffsets[2] = D3DXFROMWINEVec3Dot(&edgeNormals[2], &v[2]);
}

D3DXFROMWINEVECTOR3 planeHit(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &normal, const float offset){
	D3DXFROMWINEVECTOR3 dir = v1 - v0;
	float d = D3DXFROMWINEVec3Dot(&v0, &normal) + offset;
	D3DXFROMWINEVECTOR3 pos = v0 - (d / D3DXFROMWINEVec3Dot(&normal, &dir)) * dir;

	return pos;
}

void BTri::split(BTri *dest, int &nPos, int &nNeg, const D3DXFROMWINEVECTOR3 &normal, const float offset, const float epsilon) const {
	float d[3];
	for (int i = 0; i < 3; i++){
		d[i] = D3DXFROMWINEVec3Dot(&v[i], &normal) + offset;
	}

	int first  = 2;
	int second = 0;
	while (!(d[second] > epsilon && d[first] <= epsilon)){
		first = second;
		second++;
	}

	// Positive triangles
	nPos = 0;
	D3DXFROMWINEVECTOR3 h = planeHit(v[first], v[second], normal, offset);
	do {
		first = second;
		second++;
		if (second >= 3) second = 0;

		dest->v[0] = h;
		dest->v[1] = v[first];
		if (d[second] > epsilon){
			dest->v[2] = v[second];
		} else {
			dest->v[2] = h = planeHit(v[first], v[second], normal, offset);
		}

		dest->data = data;
		dest->setup();
		dest++;
		nPos++;
	} while (d[second] > epsilon);

	// Skip zero area triangle
	if (fabsf(d[second]) <= epsilon){
		first = second;
		second++;
		if (second >= 3) second = 0;
	}

	// Negative triangles
	nNeg = 0;
	do {
		first = second;
		second++;
		if (second >= 3) second = 0;

		dest->v[0] = h;
		dest->v[1] = v[first];
		if (d[second] < -epsilon){
			dest->v[2] = v[second];
		} else {
			dest->v[2] = planeHit(v[first], v[second], normal, offset);
		}

		dest->data = data;
		dest->setup();
		dest++;
		nNeg++;
	} while (d[second] < -epsilon);
}

bool BTri::intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1) const {
	D3DXFROMWINEVECTOR3 dir = v0 - v1;
	float k = (D3DXFROMWINEVec3Dot(&normal, &v0) + offset) / D3DXFROMWINEVec3Dot(&normal, &dir);

	if (k < 0 || k > 1) return false;

	D3DXFROMWINEVECTOR3 pos = v0 - k * dir;

	for (unsigned int i = 0; i < 3; i++){
		if (D3DXFROMWINEVec3Dot(&edgeNormals[i], &pos) < edgeOffsets[i]){
			return false;
		}
	}
	return true;
}

bool BTri::isAbove(const D3DXFROMWINEVECTOR3 &pos) const {
/*	for (unsigned int i = 0; i < 3; i++){
		if (D3DXFROMWINEVec3Dot(&edgeNormals[i], &pos) < edgeOffsets[i]){
			return false;
		}
	}
	return true;
*/

	// Tight unrolled code for better performance
	return (edgeNormals[0].x * pos.x + edgeNormals[0].y * pos.y + edgeNormals[0].z * pos.z >= edgeOffsets[0] &&
			edgeNormals[1].x * pos.x + edgeNormals[1].y * pos.y + edgeNormals[1].z * pos.z >= edgeOffsets[1] &&
			edgeNormals[2].x * pos.x + edgeNormals[2].y * pos.y + edgeNormals[2].z * pos.z >= edgeOffsets[2]);
}

BNode::~BNode(){
	delete back;
	delete front;
}

int BNode::getSize() const {
	int sum = 1;
	if (back) sum += back->getSize();
	if (front) sum += front->getSize();

	return sum;
}

bool BNode::intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &dir, D3DXFROMWINEVECTOR3 *point, const BTri **triangle) const {
	float d0 = D3DXFROMWINEVec3Dot(&v0, &tri.normal) + tri.offset;
	float d1 = D3DXFROMWINEVec3Dot(&v1, &tri.normal) + tri.offset;

	D3DXFROMWINEVECTOR3 pos;

	if (d0 > 0){
		if (d1 <= 0){
			pos = v0 - (d0 / D3DXFROMWINEVec3Dot(&tri.normal, &dir)) * dir;
		}

		if (front != NULL && front->intersects(v0, (d1 <= 0)? pos : v1, dir, point, triangle)) return true;

		if (d1 <= 0){
			if (tri.isAbove(pos)){
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (back != NULL && back->intersects(pos, v1, dir, point, triangle)) return true;
		}
	} else {
		if (d1 > 0){
			pos = v0 - (d0 / D3DXFROMWINEVec3Dot(&tri.normal, &dir)) * dir;
		}

		if (back != NULL && back->intersects(v0, (d1 > 0)? pos : v1, dir, point, triangle)) return true;

		if (d1 > 0){
			if (tri.isAbove(pos)){
				if (point) *point = pos;
				if (triangle) *triangle = &tri;
				return true;
			}
			if (front != NULL && front->intersects(pos, v1, dir, point, triangle)) return true;
		}
	}

	return false;
}

bool BNode::pushSphere(D3DXFROMWINEVECTOR3 &pos, const float radius) const {
	float d = D3DXFROMWINEVec3Dot(&pos, &tri.normal) + tri.offset;

	bool pushed = false;
	if (fabsf(d) < radius){
		if (tri.isAbove(pos)){
			// Right above the triangles
			pos += (radius - d) * tri.normal; 
			pushed = true;
		} else {
			// Near any of the edges?
			D3DXFROMWINEVECTOR3 v1 = tri.v[2];
			for (int i = 0; i < 3; i++){
				D3DXFROMWINEVECTOR3 v0 = v1;
				v1 = tri.v[i];

				D3DXFROMWINEVECTOR3 diff = v1 - v0;
				D3DXFROMWINEVECTOR3 diff_ = pos - v0;

				float t = D3DXFROMWINEVec3Dot(&diff, &diff_);
				if (t > 0){
					float f = D3DXFROMWINEVec3Dot(&diff, &diff);		
					if (t < f){
                        D3DXFROMWINEVECTOR3 vO;
						D3DXFROMWINEVECTOR3 v = v0 + (t / f) * diff;
						D3DXFROMWINEVECTOR3 dir = pos - v;
						if (D3DXFROMWINEVec3Dot(&dir, &dir) < radius * radius){
                            D3DXFROMWINEVec3Normalize(&vO, &dir);
							pos = v + radius * vO;
							break;
						}
					}
				}
			}
		}
	}

	if (front != NULL && d > -radius) pushed |= front->pushSphere(pos, radius);
	if (back  != NULL && d <  radius) pushed |= back ->pushSphere(pos, radius);

	return pushed;
}

void BNode::build(VArray_ <BTri> &tris, const int cutWeight, const int unbalanceWeight){
	float epsilon = 0.001f;

	unsigned int index = 0;
	int minScore = 0x7FFFFFFF;
	for (unsigned int i = 0; i < tris.getCount(); i++){
		int score = 0;
		int diff = 0;
		for (unsigned int k = 0; k < tris.getCount(); k++){
			unsigned int neg = 0, pos = 0;
			for (unsigned int j = 0; j < 3; j++){
				float dist = D3DXFROMWINEVec3Dot(&tris[k].v[j], &tris[i].normal) + tris[i].offset;
				if (dist < -epsilon) neg++; else
				if (dist >  epsilon) pos++;
			}
			if (pos){
				if (neg) score += cutWeight; else diff++;
			} else {
				if (neg) diff--; else diff++;
			}
		}
		score += unbalanceWeight * abs(diff);
		if (score < minScore){
			minScore = score;
			index = i;
		}
	}
	tri = tris[index];
	tris.fastRemove(index);

	VArray_ <BTri> backTris;
	VArray_ <BTri> frontTris;
	for (unsigned int i = 0; i < tris.getCount(); i++){

		unsigned int neg = 0, pos = 0;
		for (unsigned int j = 0; j < 3; j++){
			float dist = D3DXFROMWINEVec3Dot(&tris[i].v[j], &tri.normal) + tri.offset;
			if (dist < -epsilon) neg++; else
			if (dist >  epsilon) pos++;
		}

		if (neg){
			if (pos){
				BTri newTris[3];
				int nPos, nNeg;
				tris[i].split(newTris, nPos, nNeg, tri.normal, tri.offset, epsilon);
				for (int i = 0; i < nPos; i++){
					frontTris.add(newTris[i]);
				}
				for (int i = 0; i < nNeg; i++){
					backTris.add(newTris[nPos + i]);
				}
			} else {
				backTris.add(tris[i]);
			}
		} else {
			frontTris.add(tris[i]);
		}
	}
	tris.reset();

	if (backTris.getCount() > 0){
		back = new BNode;
		back->build(backTris, cutWeight, unbalanceWeight);
	} else back = NULL;

	if (frontTris.getCount() > 0){
		front = new BNode;
		front->build(frontTris, cutWeight, unbalanceWeight);
	} else front = NULL;
}

void BNode::read(FILE *file){
	fread(&tri.v, sizeof(tri.v), 1, file);
	tri.setup();

	int flags = 0;
	fread(&flags, sizeof(int), 1, file);

	if (flags & 1){
		back = new BNode;
		back->read(file);
	} else back = NULL;

	if (flags & 2){
		front = new BNode;
		front->read(file);
	} else front = NULL;
}

void BNode::write(FILE *file) const {
	fwrite(&tri.v, sizeof(tri.v), 1, file);

	int flags = 0;
	if (back)  flags |= 1;
	if (front) flags |= 2;
	fwrite(&flags, sizeof(int), 1, file);

	if (back)  back->write(file);
	if (front) front->write(file);
}

int BSP::getSize() const {
	return top->getSize();
}

void BSP::addTriangle(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &v2, void *data){
	BTri tri;

	tri.v[0] = v0;
	tri.v[1] = v1;
	tri.v[2] = v2;
	tri.data = data;

	tri.setup();

	tris.add(tri);
}

void BSP::build(const int cutWeight, const int unbalanceWeight){
	top = new BNode;

	top->build(tris, cutWeight, unbalanceWeight);
}

bool BSP::load(const char *fileName){
//LOG_FNLN;
//LOG_PRINT("fopen:%s\n", fileName);
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	delete top;
	top = new BNode;
	top->read(file);

	fclose(file);
	return true;
}

bool BSP::save(const char *fileName) const {
	if (top == NULL) return false;
//LOG_FNLN;
//LOG_PRINT("fopen:%s\n", fileName);
	FILE *file = fopen(fileName, "wb");
	if (file == NULL) return false;

	top->write(file);

	fclose(file);
	return true;
}

bool BSP::intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, D3DXFROMWINEVECTOR3 *point, const BTri **triangle) const {
	return top->intersects(v0, v1, v1 - v0, point, triangle);
}

bool BSP::pushSphere(D3DXFROMWINEVECTOR3 &pos, const float radius) const {
	if (top != NULL) return top->pushSphere(pos, radius);

	return false;
}

