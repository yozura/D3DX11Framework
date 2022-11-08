1. 정점의 Z좌표를 뒤집는다.	
> vz * -1.0f	
2. 텍스쳐 좌표의 TV를 뒤집는다.	
> 1.0f - tv	
3. 법선의 Z좌표를 뒤집는다.	
> nz * -1.0f	
4. 삼각형의 그려지는 방향을 반시계 방향(오른손 좌표계)에서 시계 방향(왼손 좌표계)으로 바꾼다.	
> right(faceX, faceY, faceZ) -> left(faceZ, faceY, faceX)	