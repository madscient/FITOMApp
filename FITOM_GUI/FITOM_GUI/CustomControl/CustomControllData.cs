using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Globalization;

namespace FITOM_GUI.CustomControl
{
    public class RangeInt
    {
        public int min = 0;
        public int max = 0;
    }
    public class RangeDouble
    {
        public double min = 0;
        public double max = 0;
    }

    [TypeConverter(typeof(BorderFlagsConverter))]
    public class BorderFlags
    {
        public bool Top { get; set; } = true;
        public bool Bottom { get; set; } = true;
        public bool Left { get; set; } = true;
        public bool Right { get; set; } = true;
    }
       
    public class BorderFlagsConverter : ExpandableObjectConverter
    {
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            return (sourceType == typeof(string)) ? true : base.CanConvertFrom(context, sourceType);
        }
        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            return (destinationType == typeof(string)) ? true : base.CanConvertTo(context, destinationType);
        }
        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
        {
            string[] src = (value as string).Split(',');
            if (src.Length < 4)
            {
                return null;
            }
            return new BorderFlags()
            {
                Top = bool.Parse(src[0]),
                Bottom = bool.Parse(src[1]),
                Left = bool.Parse(src[2]),
                Right = bool.Parse(src[3]),
            };
        }
        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            BorderFlags src = value as BorderFlags;
            return (destinationType == typeof(string) && src != null) ?
                string.Format("{0},{1},{2},{3}", src.Top, src.Bottom, src.Left, src.Right) :
                base.ConvertTo(context, culture, value, destinationType);
        }
    }

    [TypeConverter(typeof(BorderWidthsConverter))]
    public class BorderWidths
    {
        [Browsable(true)]
        public int Top { get; set; } = 0;
        [Browsable(true)]
        public int Bottom { get; set; } = 0;
        [Browsable(true)]
        public int Left { get; set; } = 0;
        [Browsable(true)]
        public int Right { get; set; } = 0;
        public BorderWidths() { }
        public BorderWidths(int top, int bottom, int left, int right)
        {
            Top = top;
            Bottom = bottom;
            Left = left;
            Right = right;
        }
    }
    public class BorderWidthsConverter : ExpandableObjectConverter
    {
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            return (sourceType == typeof(string)) ? true : base.CanConvertFrom(context, sourceType);
        }
        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            return (destinationType == typeof(string)) ? true : base.CanConvertTo(context, destinationType);
        }
        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
        {
            string[] src = (value as string).Split(',');
            if (src.Length < 4)
            {
                return null;
            }
            return new BorderWidths()
            {
                Top = int.Parse(src[0]),
                Bottom = int.Parse(src[1]),
                Left = int.Parse(src[2]),
                Right = int.Parse(src[3]),
            };
        }
        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            BorderWidths src = value as BorderWidths;
            return (destinationType == typeof(string) && src != null) ?
                string.Format("{0},{1},{2},{3}", src.Top, src.Bottom, src.Left, src.Right) :
                base.ConvertTo(context, culture, value, destinationType);
        }
    }

    class CommonProc
    {
        public static GraphicsPath GetRoundRect(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 左上の角丸
            path.AddArc(rect.Left, rect.Top,
                radius * 2, radius * 2,
                180, 90);
            // 上の線
            path.AddLine(rect.Left + radius, rect.Top,
                rect.Right - radius, rect.Top);
            // 右上の角丸
            path.AddArc(rect.Right - radius * 2, rect.Top,
                radius * 2, radius * 2,
                270, 90);
            // 右の線
            path.AddLine(rect.Right, rect.Top + radius,
                rect.Right, rect.Bottom - radius);
            // 右下の角丸
            path.AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2,
                radius * 2, radius * 2,
                0, 90);
            // 下の線
            path.AddLine(rect.Right - radius, rect.Bottom,
                rect.Left + radius, rect.Bottom);
            // 左下の角丸
            path.AddArc(rect.Left, rect.Bottom - radius * 2,
                radius * 2, radius * 2,
                90, 90);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top + radius);

            path.CloseFigure();

            return path;
        }
        //左上角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_TopLeft(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 左上の角丸
            path.AddArc(rect.Left, rect.Top,
                radius * 2, radius * 2,
                180, 90);
            // 上の線
            path.AddLine(rect.Left + radius, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left, rect.Bottom);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom,
                rect.Left, rect.Top + radius);

            path.CloseFigure();

            return path;
        }

        //右上角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_TopRight(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right - radius, rect.Top);
            // 右上の角丸
            path.AddArc(rect.Right - (radius * 2), rect.Top,
                radius * 2, radius * 2,
                270, 90);
            // 右の線
            path.AddLine(rect.Right, rect.Top + radius,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left, rect.Bottom);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        //左下角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_BottomLeft(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left + radius, rect.Bottom);
            // 左下の角丸
            path.AddArc(rect.Left, rect.Bottom - radius * 2,
                radius * 2, radius * 2,
                90, 90);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        //右下角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_BottomRight(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom - radius);
            // 右下の角丸
            path.AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2,
                radius * 2, radius * 2,
                0, 90);
            // 下の線
            path.AddLine(rect.Right - radius, rect.Bottom,
                rect.Left, rect.Bottom);

            // 左の線
            path.AddLine(rect.Left, rect.Bottom,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        public static GraphicsPath GetRoundRect2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.StartFigure();

            // 左上の角丸
            path.AddArc(rect.Left, rect.Top,
                radius * 2, radius * 2,
                180, 90);
            // 上の線
            path.AddLine(rect.Left + radius, rect.Top,
                rect.Right - radius, rect.Top);
            // 右上の角丸
            path.AddArc(rect.Right - (radius * 2) - 1, rect.Top,
                radius * 2, radius * 2,
                270, 90);
            // 右の線
            path.AddLine(rect.Right, rect.Top + radius,
                rect.Right, rect.Bottom - radius);
            // 右下の角丸
            path.AddArc(rect.Right - (radius * 2) - 1, rect.Bottom - (radius * 2) - 1,
                radius * 2, radius * 2,
                0, 90);
            // 下の線
            path.AddLine(rect.Right - radius, rect.Bottom,
                rect.Left + radius, rect.Bottom);
            // 左下の角丸
            path.AddArc(rect.Left, rect.Bottom - (radius * 2) - 1,
               radius * 2, radius * 2,
               90, 90);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top + radius);

            path.CloseFigure();

            return path;
        }

        //上半分
        public static GraphicsPath GetRoundRect_Top2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.StartFigure();

            // 左上の角丸
            path.AddArc(rect.Left, rect.Top,
                radius * 2, radius * 2,
                180, 90);
            // 上の線
            path.AddLine(rect.Left + radius, rect.Top,
                rect.Right - radius, rect.Top);
            // 右上の角丸
            path.AddArc(rect.Right - (radius * 2) - 1, rect.Top,
                radius * 2, radius * 2,
                270, 90);

            // 右の線
            path.AddLine(rect.Right, rect.Top + radius,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left, rect.Bottom);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top + radius);

            path.CloseFigure();

            return path;
        }

        //下半分
        public static GraphicsPath GetRoundRect_Bottom2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom - radius);
            // 右下の角丸
            path.AddArc(rect.Right - (radius * 2) - 1, rect.Bottom - (radius * 2) - 1,
                radius * 2, radius * 2,
                0, 90);
            // 下の線
            path.AddLine(rect.Right - radius, rect.Bottom,
                rect.Left + radius, rect.Bottom);
            // 左下の角丸
            path.AddArc(rect.Left, rect.Bottom - (radius * 2) - 1,
               radius * 2, radius * 2,
               90, 90);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top);
            path.CloseFigure();

            return path;
        }


        //右上角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_TopRight2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right - radius, rect.Top);

            // 右上の角丸
            path.AddArc(rect.Right - (radius * 2) - 1, rect.Top,
                radius * 2, radius * 2,
                270, 90);
            // 右の線
            path.AddLine(rect.Right, rect.Top + radius,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left, rect.Bottom);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        //左下角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_BottomLeft2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom);
            // 下の線
            path.AddLine(rect.Right, rect.Bottom,
                rect.Left + radius, rect.Bottom);
            // 左下の角丸
            path.AddArc(rect.Left, rect.Bottom - (radius * 2) - 1,
                radius * 2, radius * 2,
                90, 90);
            // 左の線
            path.AddLine(rect.Left, rect.Bottom - radius,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        //右下角丸長方形のGraphicsPathを作成
        public static GraphicsPath GetRoundRect_BottomRight2(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();

            path.StartFigure();

            // 上の線
            path.AddLine(rect.Left, rect.Top,
                rect.Right, rect.Top);
            // 右の線
            path.AddLine(rect.Right, rect.Top,
                rect.Right, rect.Bottom - radius);
            // 右下の角丸
            path.AddArc(rect.Right - radius * 2 - 1, rect.Bottom - radius * 2 - 1,
                radius * 2, radius * 2,
                0, 90);
            // 下の線
            path.AddLine(rect.Right - radius, rect.Bottom,
                rect.Left, rect.Bottom);

            // 左の線
            path.AddLine(rect.Left, rect.Bottom,
                rect.Left, rect.Top);

            path.CloseFigure();

            return path;
        }

        public static StringFormat ConvertAlignmentProperty(ContentAlignment align)
        {
            StringFormat cFormat = new StringFormat(StringFormat.GenericTypographic);
            Int32 lNum = (Int32)Math.Log((Double)align, 2);
            cFormat.LineAlignment = (StringAlignment)(lNum / 4);
            cFormat.Alignment = (StringAlignment)(lNum % 4);
            return cFormat;
        }

    }
}
