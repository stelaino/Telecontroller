<%@ page language="java" contentType="text/html; charset=GB18030"
    pageEncoding="UTF-8"%>
<%@ page import="com.hub.demo.cket" %>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Light控制</title>
</head>
<% request.setCharacterEncoding("UTF-8"); %>
<body>
<%
	cket ck=new cket();
	ck.close();
%>
<form action="index.jsp" >
<div align="center">
	<input type="submit" value="打开" style="width:800px;height:260px;font-size:200px;"/>
</div>	
</form>
</body>
</html>