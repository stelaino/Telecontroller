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
	ck.open();
%>
<form action="open.jsp">
<div align="center">
	<input type="submit" value="关闭" style="width:800px;height:260px;font-size:200px;"/>
</div>	
</form>
</body>
</html>