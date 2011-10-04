using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RobotSensorStream
{
    public partial class frmSensorStream : Form
    {
        const int MAX_GRAPH_VALUES = 10;

        Stack<Int32>[] Orientation = new Stack<Int32>[3];
        Stack<Int32>[] Direction = new Stack<Int32>[3];
        Stack<Int32>[] Acceleration = new Stack<Int32>[3];
        Stack<Int32> Temperature;
        Stack<Int32> Pressure;

        void Log(String text)
        {
            txtLog.Text += "\r\n" + text.Replace("\r\n", "");
            txtLog.SelectionStart = txtLog.Text.Length;
            txtLog.ScrollToCaret();
            this.Update();
        }

        public frmSensorStream()
        {
            InitializeComponent();
        }

        private void frmSensorStream_Load(object sender, EventArgs e)
        {
            String[] PortNames = System.IO.Ports.SerialPort.GetPortNames();
            Array.Sort<String>(PortNames, delegate(string strA, string strB) { return int.Parse(strA.Substring(3)).CompareTo(int.Parse(strB.Substring(3))); });
            cmbPort.Items.AddRange(PortNames);

            spSerialPort.NewLine = "\r\n";
            spSerialPort.ReadBufferSize = 1024;

            for (int i = 0; i < 3; i++)
            {
                Orientation[i] = new Stack<int>();
                Direction[i] = new Stack<int>();
                Acceleration[i] = new Stack<int>();
            }

            Temperature = new Stack<int>();
            Pressure = new Stack<int>();

            chtOrientation.ChartAreas[0].AxisY.Minimum = -1024;
            chtOrientation.ChartAreas[0].AxisY.Maximum = 1024;

            chtDirection.ChartAreas[0].AxisY.Minimum = -1024;
            chtDirection.ChartAreas[0].AxisY.Maximum = 1024;

            chtAcceleration.ChartAreas[0].AxisY.Minimum = -1024;
            chtAcceleration.ChartAreas[0].AxisY.Maximum = 1024;

            chtTemperature.ChartAreas[0].AxisY.Maximum = 60;
            chtPressure.ChartAreas[0].AxisY.Maximum = 65536;
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (cmbPort.Text.Length == 0)
                return;

            if (spSerialPort.IsOpen)
            {
                Log("Closing port " + cmbPort.Text);
                spSerialPort.Close();
            }

            try
            {
                Log("Opening port " + cmbPort.Text + "...");
                spSerialPort.PortName = cmbPort.Text;

                spSerialPort.Open();
                Log("Opened port " + cmbPort.Text);
            }
            catch (Exception ex)
            {
                Log("Could not open port " + cmbPort.Text + " (" + ex.Message + ")");
                return;
            }
        }

        private void spSerialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            String SensorDataLine = spSerialPort.ReadLine();
            String[] SensorValues = SensorDataLine.Split(new char[] { ',' });

            Int32 CurrentSensorValue;

            for (int i = 0; i < 3; i++)
            {
                if (Int32.TryParse(SensorValues[i], out CurrentSensorValue))
                    Direction[i].Push(CurrentSensorValue);

                if (Int32.TryParse(SensorValues[3 + i], out CurrentSensorValue))
                    Acceleration[i].Push(CurrentSensorValue);

                if (Int32.TryParse(SensorValues[6 + i], out CurrentSensorValue))
                    Orientation[i].Push(CurrentSensorValue);
            }

            if (Int32.TryParse(SensorValues[9], out CurrentSensorValue))
                Pressure.Push(CurrentSensorValue);

            if (Int32.TryParse(SensorValues[10], out CurrentSensorValue))
                Temperature.Push(CurrentSensorValue);
        }

        private void tmrUpdate_Tick(object sender, EventArgs e)
        {
            for (int i = 0; i < 3; i++)
            {
                while (Orientation[i].Count > 0)
                {
                    chtOrientation.Series[i].Points.AddY(Orientation[i].Pop());

                    if (chtOrientation.Series[i].Points.Count > MAX_GRAPH_VALUES)
                        chtOrientation.Series[i].Points.RemoveAt(0);
                }

                while (Direction[i].Count > 0)
                {
                    chtDirection.Series[i].Points.AddY(Direction[i].Pop());

                    if (chtDirection.Series[i].Points.Count > MAX_GRAPH_VALUES)
                        chtDirection.Series[i].Points.RemoveAt(0);
                }

                while (Acceleration[i].Count > 0)
                {
                    chtAcceleration.Series[i].Points.AddY(Acceleration[i].Pop());

                    if (chtAcceleration.Series[i].Points.Count > MAX_GRAPH_VALUES)
                        chtAcceleration.Series[i].Points.RemoveAt(0);
                }
            }

            while (Temperature.Count > 0)
            {
                chtTemperature.Series[0].Points.AddY(Temperature.Pop());

                if (chtTemperature.Series[0].Points.Count > MAX_GRAPH_VALUES)
                    chtTemperature.Series[0].Points.RemoveAt(0);
            }

            while (Pressure.Count > 0)
            {
                chtPressure.Series[0].Points.AddY(Pressure.Pop());

                if (chtPressure.Series[0].Points.Count > MAX_GRAPH_VALUES)
                    chtPressure.Series[0].Points.RemoveAt(0);
            }
        }
    }
}
