/* struct: $(Struct.NsName.!dot) */
public class $(Struct.Name)\
#ifneq($(Struct.ParentName),)
 extends $(Struct.ParentName)\
#ifeqend

{
#ifneq($(Struct.Structs.$Count),0)
#indent +
#foreach $(Struct.Structs)
#cginclude "StructOnly.java"

#end
#indent -
#ifeqend
\
#foreach $(Struct.Members)
  public \
#ifeq($(Param.DataType.Type),string)
String\
#else
#ifeq($(Param.DataType.Type),dataobject)
Node\
#else
#ifeq($(Param.DataType.Name),bool)
boolean\
#else
$(Param.DataType.UsedName.!dot)\
#ifeqend
#ifeqend
#ifeqend
 $(Param.Name);
#end

  public Node Serialize(Element tElement)
  {
#ifneq($(Struct.ParentName),)
    super.Serialize(tElement);

#ifeqend
#foreach $(Struct.Members)
    Element tParam$(Param.Name) = tElement.createElement("", "$(Param.Name)");
#ifeq($(Param.DataType.Type),generic)
    tParam$(Param.Name).addChild(Node.TEXT, "" + $(Param.Name));
#else
#ifeq($(Param.DataType.Type),string)
    tParam$(Param.Name).addChild(Node.TEXT, $(Param.Name));
#else
#ifeq($(Param.DataType.Type),dataobject)
    tParam$(Param.Name).addChild(Node.ELEMENT, $(Param.Name));
#else
#ifeq($(Param.DataType.Type),struct||typedef)
    $(Param.Name).Serialize(tElement.createElement("", "$(Param.Name)"));
#else
#cgerror "Param.DataType.Type = $(Param.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
    tElement.addChild(Node.ELEMENT, tParam$(Param.Name));
#end
    return tElement;
  }

  public Object Deserialize(Element tElement)
  {
#ifneq($(Struct.ParentName),)
    super.Deserialize(tElement);

#ifeqend
#foreach $(Struct.Members)
#ifeq($(Param.DataType.Type),generic)
#ifeq($(Param.DataType.Name),byte)
    $(Param.Name) = Byte.parseByte(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),int)
    $(Param.Name) = Integer.parseInt(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),short)
    $(Param.Name) = Short.parseShort(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),long)
    $(Param.Name) = Long.parseLong(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),float)
    $(Param.Name) = Float.parseFloat(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),double)
    $(Param.Name) = Double.parseDouble(tElement.getElement("", "$(Param.Name)").getText(0));
#else
#ifeq($(Param.DataType.Name),bool)
    $(Param.Name) = tElement.getElement("", "$(Param.Name)").getText(0).equalsIgnoreCase("true") || tElement.getElement("", "$(Param.Name)").getText(0).equals("1");
#else
#cgerror generic return type "$(Param.DataType.Name)" is not supported
#ifeqend // bool
#ifeqend // double
#ifeqend // float
#ifeqend // long
#ifeqend // short
#ifeqend // int
#ifeqend // byte
#else   // generic
#ifeq($(Param.DataType.Type),string)
    $(Param.Name) = tElement.getElement("", "$(Param.Name)").getText(0);
#else
#ifeq($(Param.DataType.Type),dataobject)
    $(Param.Name) = tElement.getElement("", "$(Param.Name)");
#else
#ifeq($(Param.DataType.Type),struct||typedef)
    $(Param.Name) = new $(Param.DataType.UsedName.!dot)();
    $(Param.Name).Deserialize(tElement.getElement("", "$(Param.Name)"));
#else
#cgerror "Param.DataType.Type = $(Param.DataType.Type);"
#ifeqend
#ifeqend
#ifeqend
#ifeqend
#end
    return this;
  }
}

