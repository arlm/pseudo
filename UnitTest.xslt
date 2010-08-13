<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" >
<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="TestResults">
  <xsl:text>Run: </xsl:text>
  <xsl:value-of select="@Total"/>
  <xsl:text>, Failures: </xsl:text>
  <xsl:value-of select="@Failures"/>
  <xsl:text>, Not Run: </xsl:text>
  <xsl:value-of select="@NotRun"/>
  <xsl:text>, Total Time: </xsl:text>
  <xsl:value-of select="format-number(sum(//TestClass/@Time), '#.000')" />
  <xsl:text> seconds</xsl:text>
  <xsl:text disable-output-escaping='yes'>&#xA;&#xA;</xsl:text>
  <xsl:if test="//TestMethod[Failure]">
    <xsl:text>Failures:</xsl:text><xsl:text disable-output-escaping='yes'>&#xA;</xsl:text>
  </xsl:if>
  <xsl:apply-templates select="//TestMethod[Failure]"/>
  <xsl:if test="//TestMethod[@Executed='False']">
    <xsl:text>Tests Not Run:</xsl:text><xsl:text disable-output-escaping='yes'>&#xA;</xsl:text>
  </xsl:if>
  <xsl:apply-templates select="//TestMethod[@Executed='False']"/>
</xsl:template>

<xsl:template match="TestMethod">
  <xsl:value-of select="position()"/><xsl:text>) </xsl:text>
  <xsl:value-of select="@Name"/>
  <xsl:text> : </xsl:text>
  <xsl:value-of disable-output-escaping="yes" select="child::node()/Message"/>
  <xsl:text disable-output-escaping='yes'>&#xA;</xsl:text>
  <xsl:apply-templates select="Failure" />
</xsl:template>

<xsl:template match="Failure">
  <xsl:for-each select="child::node()/StackFrame">
    <xsl:sort select="position()" order="ascending" data-type="number" />
    <xsl:text>&#x20;&#x20;</xsl:text>
    <xsl:choose>
      <xsl:when test="@FileName = ''">
        <xsl:value-of select="@ModuleName" /><xsl:text>:&#x20;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@FileName" /><xsl:text>(</xsl:text><xsl:value-of select="@Line" /><xsl:text>):&#x20;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of disable-output-escaping="yes" select="@MethodName" /><xsl:text disable-output-escaping="yes">&#xA;</xsl:text>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>

  