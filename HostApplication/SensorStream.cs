using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace RobotSensorStream
{
    public partial class frmSensorStream : Form
    {
        const int MAX_GRAPH_VALUES = 10;

        Stack<String> ReceivedLines = new Stack<String>();

        void Log(String text)
        {
            txtLog.Text += "\r\n" + text.Replace("\r\n", "");
            txtLog.SelectionStart = txtLog.Text.Length;
            txtLog.ScrollToCaret();
            this.Update();
        }

        void AddChartPoint(DataPointCollection chartPoints, Double value)
        {
            chartPoints.AddY(value);

            if (chartPoints.Count > MAX_GRAPH_VALUES)
                chartPoints.RemoveAt(0);
        }

        void UpdateSerialPortList()
        {
            String[] PortNames = System.IO.Ports.SerialPort.GetPortNames();
            Array.Sort<String>(PortNames, delegate(string strA, string strB) { return int.Parse(strA.Substring(3)).CompareTo(int.Parse(strB.Substring(3))); });
            cmbPort.Items.Clear();
            cmbPort.Items.AddRange(PortNames);
        }

        public frmSensorStream()
        {
            InitializeComponent();
        }

        private void frmSensorStream_Load(object sender, EventArgs e)
        {
            spSerialPort.NewLine = "\r\n";
            spSerialPort.ReadBufferSize = 1024;

            UpdateSerialPortList();
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

            ReceivedLines.Clear();

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
            ReceivedLines.Push(spSerialPort.ReadLine());
        }

        private void tmrUpdate_Tick(object sender, EventArgs e)
        {
            if (ReceivedLines.Count == 0)
                return;

            // Line Format: <Compass X>, <Compass Y>, <Compass Z>, <Acc X>, <Acc Y>, <Acc Z>,  <Gyro X>, <Gyro Y>, <Gyro Z>, Pressure, Temperature
            Log("DATA> " + ReceivedLines.Peek());

            String[] SensorValues = ReceivedLines.Pop().Split(new char[] { ',' });
            Double CurrentSensorValue;

            /* Convert compass data to human readable units and add it to the graph */
            for (int i = 0; i < 3; i++)
            {
                if (Double.TryParse(SensorValues[0 + i], out CurrentSensorValue))
                  AddChartPoint(chtDirection.Series[i].Points, ((CurrentSensorValue / (double)0xFFF) * chtDirection.ChartAreas[0].AxisY.Maximum));
            }

            /* Convert accelerometer data to human readable units and add it to the graph */
            for (int i = 0; i < 3; i++)
            {
                if (Double.TryParse(SensorValues[3 + i], out CurrentSensorValue))
                    AddChartPoint(chtAcceleration.Series[i].Points, ((CurrentSensorValue / (double)0x1FF) * chtAcceleration.ChartAreas[0].AxisY.Maximum));
            }

            /* Convert gyroscope data to human readable units and add it to the graph */
            for (int i = 0; i < 3; i++)
            {
                if (Double.TryParse(SensorValues[6 + i], out CurrentSensorValue))
                    AddChartPoint(chtOrientation.Series[i].Points, (CurrentSensorValue / (double)14.375));
            }

            /* Convert pressure data to human readable units and add it to the graph */
            if (Double.TryParse(SensorValues[9], out CurrentSensorValue))
                AddChartPoint(chtPressure.Series[0].Points, CurrentSensorValue);

            /* Convert temperature data to human readable units and add it to the graph */
            if (Double.TryParse(SensorValues[10], out CurrentSensorValue))
                AddChartPoint(chtTemperature.Series[0].Points, (35 + ((13200 + CurrentSensorValue) / (double)280)));
        }

        private void cmbPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnConnect.Enabled = (cmbPort.Text.Length != 0);
        }

        private void cmbPort_TextChanged(object sender, EventArgs e)
        {
            btnConnect.Enabled = (cmbPort.Text.Length != 0);
        }

        private void btnRefreshPorts_Click(object sender, EventArgs e)
        {
            UpdateSerialPortList();
        }

        private void llblLUFALib_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(llblLUFALib.Text);
        }

        private void llblFourWalledCubicle_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(llblFourWalledCubicle.Text);
        }
    }
}
