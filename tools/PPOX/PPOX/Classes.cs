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
        private List<Point> collisionPoints;
        public List<Point> CollisionPoints
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
            collisionPoints = new List<Point>();
            selectedPoint = -1;
            filePath = _filepath;
        }

        //should be only calling this function after already creating the object
        public void LoadXML(XmlTextReader textReader)
        {
            //reading width and height
            int width = int.Parse(textReader.ReadElementString());
            int height = int.Parse(textReader.ReadElementString());
            textReader.ReadEndElement();

            //reading position
            Position = new Point(int.Parse(textReader.ReadElementString()) - (width / 2), int.Parse(textReader.ReadElementString()) - (height / 2));
            textReader.ReadEndElement();

            //reading properties
            int numProp = int.Parse(textReader.ReadElementString());
            for (int i = 0; i < numProp; ++i)
                Properties.Add(new Property(textReader.Name, textReader.ReadElementString()));
            textReader.ReadEndElement();

            //reading collision points
            int numVerts = int.Parse(textReader.ReadElementString());
            for (int i = 0; i < numVerts; ++i)
            {
                CollisionPoints.Add(
                    new Point(
                        (Position.X + int.Parse(textReader.ReadElementString())),
                        (Position.Y + int.Parse(textReader.ReadElementString()))));
                textReader.ReadEndElement();
            }
            textReader.ReadEndElement();

            //end of the object
            textReader.ReadEndElement();
        }

        public void SaveXML(XmlTextWriter textWriter)
        {           
            textWriter.WriteStartElement("Object");

            textWriter.WriteStartElement("Image_Path", "");
            textWriter.WriteString(FilePath);
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Size");
            textWriter.WriteStartElement("Width", "");
            textWriter.WriteString(Image.Size.Width.ToString());
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Height", "");
            textWriter.WriteString(Image.Size.Height.ToString());
            textWriter.WriteEndElement();
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Position");
            textWriter.WriteStartElement("X", "");
            textWriter.WriteString((Position.X + (Image.Size.Width / 2)).ToString());
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Y", "");
            textWriter.WriteString((Position.Y + (Image.Size.Height / 2)).ToString());
            textWriter.WriteEndElement();
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Properties");
            textWriter.WriteStartElement("Number_of_Properties", "");
            textWriter.WriteString(Properties.Count.ToString());
            textWriter.WriteEndElement();
            foreach (Property p in Properties)
            {
                textWriter.WriteStartElement(p.Name, "");
                textWriter.WriteString(p.Value);
                textWriter.WriteEndElement();
            }
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Collision_Geometry");
            textWriter.WriteStartElement("Number_of_Verts", "");
            textWriter.WriteString(CollisionPoints.Count.ToString());
            textWriter.WriteEndElement();
            foreach (Point p in CollisionPoints)
            {
                textWriter.WriteStartElement("Vertex");

                textWriter.WriteStartElement("X", "");
                textWriter.WriteString((p.X - (Position.X)).ToString());
                textWriter.WriteEndElement();

                textWriter.WriteStartElement("Y", "");
                textWriter.WriteString((p.Y - (Position.Y)).ToString());
                textWriter.WriteEndElement();

                textWriter.WriteEndElement();
            }
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
                    XmlTextWriter textWriter = new XmlTextWriter(_fileName, null);
                    textWriter.WriteStartDocument();
                    SaveXML(textWriter);
                    textWriter.WriteEndDocument();
                    textWriter.Close();
                }
            }
            else if (CollisionPoints.Count < 3)
            {
                if (MessageBox.Show("This object does not have at least 3 verts for the collision geometry, would you like to export anyways?",
                        "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning)
                        == System.Windows.Forms.DialogResult.Yes)
                {
                    //save
                    XmlTextWriter textWriter = new XmlTextWriter(_fileName, null);
                    textWriter.WriteStartDocument();
                    SaveXML(textWriter);
                    textWriter.WriteEndDocument();
                    textWriter.Close();
                }
            }
            else
            {
                //save
                XmlTextWriter textWriter = new XmlTextWriter(_fileName, null);
                textWriter.WriteStartDocument();
                SaveXML(textWriter);
                textWriter.WriteEndDocument();
                textWriter.Close();
            }
        }
    }
}
