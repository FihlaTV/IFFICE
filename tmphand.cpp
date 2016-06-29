double DeskActiveRange1[3];	//左下近
double DeskActiveRange2[3];	//右上远

int ActiveID;
int OnHandpadID;

void   CheckHandpad(void);

static void CheckHandpad()
{
	int i=0;
	for(i=0;i<obj_num;i++){
		if( handpad_exist == 0 &&
			object[i].mat_to_world3[0][3] > DeskActiveRange1[0] && object[i].mat_to_world3[0][3] < DeskActiveRange2[0] &&
			object[i].mat_to_world3[1][3] > DeskActiveRange1[1] && object[i].mat_to_world3[1][3] < DeskActiveRange2[1] &&
			object[i].mat_to_world3[2][3] > DeskActiveRange1[2] && object[i].mat_to_world3[2][3] < DeskActiveRange2[2])
			{
				ActiveID = i;
				break;
			}
		}
	if ( handpad_exist == 0 && i == obj_num){
		ActiveID = -1;
	}
	printf("%d",ActiveID);


	for(i=0;i<obj_num;i++){
	arUtilMatMul( world_to_cam3, object[i].mat_to_world3, object[i].mat_to_cam3 );
	argConvGlpara(object[i].mat_to_cam3, object[i].mat_to_cam4);
	//计算物体i到手持面板的距离
		object[i].dis[0] = object[i].mat_to_cam3[0][3] - handpad_to_cam3[0][3];
		object[i].dis[1] = object[i].mat_to_cam3[1][3] - handpad_to_cam3[1][3];
		object[i].dis[2] = object[i].mat_to_cam3[2][3] - handpad_to_cam3[2][3];
		object[i].pos = 0;
		if(( ActiveID == -1 ) && (handpad_exist == 1) && ((OnHandpadID == -1)||(OnHandpadID == i) )
			&& object[i].dis[0]>object[i].ActiveRange1[0] && object[i].dis[0]<object[i].ActiveRange2[0] && object[i].dis[1]>object[i].ActiveRange1[1] 
			&& object[i].dis[1]<object[i].ActiveRange2[1] && object[i].dis[2]>object[i].ActiveRange1[2] && object[i].dis[2]<object[i].ActiveRange2[2]){
			object[i].pos = 1;		//物体1已经不在世界坐标系里绘制，在手持面板里绘制
			OnHandpadID = i;
			arUtilMatInv( world_to_cam3, cam_to_world3);
			arUtilMatMul( cam_to_world3, handpad_to_cam3, object[i].mat_to_world3 );			//刷新物品1在世界坐标系中的位置
			argConvGlpara( object[i].mat_to_world3, object[i].mat_to_world4);
		}
		else {
			OnHandpadID = -1;
		}
	}
}

static void	 ObjectGohome(void)
{
	if (ActiveID != -1){
		//SetFly(0, 1, 100, 2, object[ActiveID].mat_to_world_reset);
		SetFly(ActiveID, 1001, 1050, 1, object[ActiveID].mat_to_world_reset);
	}
}
