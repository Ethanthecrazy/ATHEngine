using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;

namespace WindowsFormsApplication1
{
    public struct Property
    {
        private string name;
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        private string value;
        public string Value
        {
            get { return value; }
            set { this.value = value; }
        }

        public Property(string _Name, string _Value)
        {
            name = _Name;
            value = _Value;
        }

        public override string ToString()
        {
            return name + ": " + value;
        }

        public string[] ToMyArray()
        {
            string[] arrProp = new string[2];
            arrProp[0] = name;
            arrProp[1] = value;

            return arrProp;
        }
    }

    

    public class cObject
    {
        private List<PointF> collisionPoints;
        public List<PointF> CollisionPoints
        {
            get { return collisionPoints; }
            set { collisionPoints = value; }
        }

        string filePath;
        public string FilePath
        {
            get { return filePath; }
            set { filePath = value; }
        }

        private int selectedPoint;
        public int SelectedPoint
        {
            get { return selectedPoint; }
            set { selectedPoint = value; }
        }

        private Bitmap image;
        public Bitmap Image
        {
            get { return image; }
            set { image = value; }
        }

        private Point position;
        public Point Position
        {
            get { return position; }
            set { position = value; }
        }

        private List<Property> properties;
        public List<Property> Properties
        {
            get { return properties; }
            set { properties = value; }
        }

        public Rectangle Rectangle
        {
            get {return new Rectangle(position, image.Size); }
        }

        public Point Center
        {
            get { return new Point(position.X + image.Size.Width / 2, position.Y + image.Size.Height / 2); }
        }

        public cObject(Bitmap _image, string _filepath)
        {
            image = _image;
            position = new Point(0, 0);
            properties = new List<Property>();
            collisionPoints = new List<PointF>();
            selectedPoint = -1;
            filePath = _filepath;
        }

        //should be only calling this function after already creating the object
        public void LoadXML(XmlNode objectNode)
        {
            XmlNode objSize = objectNode.FirstChild;            

            //reading width and height
            int width = int.Parse(objSize.Attributes["Width"].Value);
            int height = int.Parse(objSize.Attributes["Height"].Value);


            //reading position
            XmlNode objPos = objSize.NextSibling;
            Position = new Point(int.Parse(objPos.Attributes["X"].Value) - (width / 2), int.Parse(objPos.Attributes["Y"].Value) - (height / 2));


            //reading properties
            XmlNode objProperties = objPos.NextSibling;
            int numProp = int.Parse(objProperties.Attributes["Number_of_Properties"].Value);

            XmlNodeList propertiesChildren = objProperties.ChildNodes;
            for (int i = 0; i < numProp; ++i)
                Properties.Add(new Property(propertiesChildren[i].Name, propertiesChildren[i].FirstChild.Value));


            //reading collision points
            XmlNode objVerts = objProperties.NextSibling;
            int numVerts = int.Parse(objVerts.Attributes["Number_of_Verts"].Value);

            XmlNodeList vertsChildren = objVerts.ChildNodes;
            for (int i = 0; i < numVerts; ++i)
            {
                 CollisionPoints.Add(
                    new Point(
                        (Position.X + (width / 2) + int.Parse(vertsChildren[i].Attributes["X"].Value)),
                        (Position.Y + (height / 2) + int.Parse(vertsChildren[i].Attributes["Y"].Value))
                        ));
            }
        }

        public void SaveXML(XmlTextWriter textWriter)
        {
            textWriter.WriteStartElement("Object");
            textWriter.WriteAttributeString("Image_Path", FilePath);

            textWriter.WriteStartElement("Size");
            textWriter.WriteAttributeString("Height", Image.Size.Height.ToString());
            textWriter.WriteAttributeString("Width", Image.Size.Width.ToString());
            textWriter.WriteEndElement();


            textWriter.WriteStartElement("Position");
            textWriter.WriteAttributeString("Y", (Center.Y).ToString());
            textWriter.WriteAttributeString("X", (Center.X).ToString());
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Properties");
            textWriter.WriteAttributeString("Number_of_Properties", Properties.Count.ToString());
            foreach (Property p in Properties)
            {
                textWriter.WriteStartElement(p.Name, "");
                textWriter.WriteString(p.Value);
                textWriter.WriteEndElement();
            }
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Collision_Geometry");
            textWriter.WriteAttributeString("Number_of_Verts", CollisionPoints.Count.ToString());

            int i, j;
            float sum = 0;
            for (i = 0; i < CollisionPoints.Count; ++i)
            {
                j = i + 1;
                if (j > CollisionPoints.Count - 1)
                    j = 0;

                sum += (CollisionPoints[j].X - CollisionPoints[i].X) * (CollisionPoints[j].Y + CollisionPoints[i].Y);
            }

            if (sum < 0) //if counter-clock-wise
            {
                for (int v = 0; v < CollisionPoints.Count; ++v)
                {
                    textWriter.WriteStartElement("Vertex");
                    textWriter.WriteAttributeString("Y", (CollisionPoints[v].Y - (Center.Y)).ToString());
                    textWriter.WriteAttributeString("X", (CollisionPoints[v].X - (Center.X)).ToString());
                    textWriter.WriteEndElement();
                }
            }
            else
            {
                for (int v = CollisionPoints.Count - 1; v > -1; --v)
                {
                    textWriter.WriteStartElement("Vertex");
                    textWriter.WriteAttributeString("Y", (CollisionPoints[v].Y - (Center.Y)).ToString());
                    textWriter.WriteAttributeString("X", (CollisionPoints[v].X - (Center.X)).ToString());
                    textWriter.WriteEndElement();
                }
            }

            textWriter.WriteEndElement();
            textWriter.WriteEndElement();
        }

        public void Export(string _fileName)
        {
            if (CollisionPoints.Count > 3 && !WindowsFormsApplication1.MainForm.IsConvex(CollisionPoints))
            {
                if (MessageBox.Show("This object is not convex, would you like to export anyways?",
                    "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning)
                    == System.Windows.Forms.DialogResult.Yes)
                {
                    //save
                    Export_Helper(_fileName);
                }
            }
            else if (CollisionPoints.Count < 3)
            {
                if (MessageBox.Show("This object does not have at least 3 verts for the collision geometry, would you like to export anyways?",
                        "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning)
                        == System.Windows.Forms.DialogResult.Yes)
                {
                    //save
                    Export_Helper(_fileName);
                }
            }
            else
            {
                //save
                Export_Helper(_fileName);
            }
        }

        private void Export_Helper(string _fileName)
        {
            XmlTextWriter textWriter = new XmlTextWriter(_fileName, null);
            textWriter.WriteStartDocument();

            textWriter.WriteStartElement("Exported_Object");
            textWriter.WriteAttributeString("Version", MainForm.VERSION);

            SaveXML(textWriter);

            textWriter.WriteEndElement();
            textWriter.WriteEndDocument();
            textWriter.Close();
        }

    }
}
