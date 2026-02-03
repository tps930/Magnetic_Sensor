using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace SENSOR_LOGGER
{
    public struct SenserStatus
    {
        public bool Road_Temp_Map { get; set; }
        public bool Ground_Temp1_Map { get; set; }
        public bool Ground_Temp2_Map { get; set; }
        public bool Freezing_Temp_Map { get; set; }
        public bool WFH_Map { get; set; }
        public bool WFH_um { get; set; }
        public bool WFH_um_BAS { get; set; }
        public bool NaCl_Map { get; set; }
        public bool Road_Temp_C { get; set; }
        public bool Road_Temp_F { get; set; }
        public bool Ground_Temp1_C { get; set; }
        public bool Ground_Temp1_F { get; set; }
        public bool Ground_Temp2_C { get; set; }
        public bool Ground_Temp2_F { get; set; }
        public bool Fresszing_Temp_C { get; set; }
        public bool Fresszing_Temp_F { get; set; }
        public bool Fricition_Coefficient { get; set; }
        public bool WFH_mils { get; set; }
        public bool WFH_mils_BAS { get; set; }
        public bool Salt_Concentration { get; set; }
        public bool Road_Conditon_RVS { get; set; }
        public bool Road_Condition_BAS70 { get; set; }
        public bool Road_Condition_BAS79 { get; set; }
        public bool Road_Condition1 { get; set; }
        public bool Road_Condition2 { get; set; }
    }

    class SensorValue
    {
        public ushort Road_Temp_Map { get; set; }
        public ushort Ground_Temp1_Map { get; set; }
        public ushort Ground_Temp2_Map { get; set; }
        public ushort Freezing_Temp_Map { get; set; }
        public ushort WFH_Map { get; set; }
        public ushort WFH_um { get; set; }
        public ushort WFH_um_BAS { get; set; }
        public ushort NaCl_Map { get; set; }
        public float Road_Temp_C { get; set; }
        public float Road_Temp_F { get; set; }
        public float Ground_Temp1_C { get; set; }
        public float Ground_Temp1_F { get; set; }
        public float Ground_Temp2_C { get; set; }
        public float Ground_Temp2_F { get; set; }
        public float Fresszing_Temp_C { get; set; }
        public float Fresszing_Temp_F { get; set; }
        public float Fricition_Coefficient { get; set; }
        public float WFH_mils { get; set; }
        public float WFH_mils_BAS { get; set; }
        public float Salt_Concentration { get; set; }
        public char Road_Conditon_RVS { get; set; }
        public char Road_Condition_BAS70 { get; set; }
        public char Road_Condition_BAS79 { get; set; }
        public char Road_Condition1 { get; set; }
        public char Road_Condition2 { get; set; }

        public short x { get; set; }

        public short y { get; set; }
        public short z { get; set; }

        public double heading { get; set; }

        public ushort traffic_60sec { get; set; }

        public byte speed { get; set; }

        public short rel { get; set; }          // z - Z0
        public short abs_rel { get; set; }      // |rel|
        public short threshold_T { get; set; }  // 임계값 T


        public byte state { get; set; }        // 0: WAIT_FALL, 1: WAIT_RISE

        public double calc_speed { get; set; }

        public DateTime RecvTime { get; set; }

        public void Clear()
        {
            Road_Temp_Map = 0;
            Ground_Temp1_Map = 0;
            Ground_Temp2_Map = 0;
            Freezing_Temp_Map = 0;
            WFH_Map = 0;
            WFH_um = 0;
            WFH_um_BAS = 0;
            NaCl_Map = 0;
            Road_Temp_C = 0;
            Road_Temp_F = 0;
            Ground_Temp1_C = 0;
            Ground_Temp1_F = 0;
            Ground_Temp2_C = 0;
            Ground_Temp2_F = 0;
            Fresszing_Temp_C = 0;
            Fresszing_Temp_F = 0;
            Fricition_Coefficient = 0;
            WFH_mils = 0;
            WFH_mils_BAS = 0;
            Salt_Concentration = 0;
            Road_Conditon_RVS = ' ';
            Road_Condition_BAS70 = ' ';
            Road_Condition_BAS79 = ' ';
            Road_Condition1 = ' ';
            Road_Condition2 = ' ';
            x = 0;
            y = 0;
            z = 0;
            heading = 0;
            traffic_60sec = 0;
            speed = 0;
            rel = 0;
            abs_rel = 0;
            threshold_T = 0;
            state = 0;
            RecvTime = new DateTime();
            calc_speed = 0.0;
    }
    }
}
