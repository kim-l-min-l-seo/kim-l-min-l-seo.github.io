---
layout:     BLACKCODE
title:      "DB 트래픽 분산을 위한 Read/Write 분기 처리"
subtitle:   ""
description: ""
date:       2023-03-30 1:10:01
author:     "MADness"
header-img: "assets/owner/hero/home-bg.jpg"
header-video: "assets/video/metrix.mp4"
thumbnail: /assets/owner/blog/thumbs/thumb01.png
tags: [SpringBoot]
category: [SpringBoot]

# img url : ![img](/assets/category/Azure/2023/02/08/01.PNG)
---

# Spring Boot DB Read/Write 분기처리
## 1. Spring Boot Project 생성
- Spring Boot
    - ver : 2.7
    - JDK : 11
    - war, jar 상관없음

- DB
    - Azure Database For MySQL
    - master
    - master 복재(read용)

## 2. 의존성 설정
```
plugins {
	id 'java'
	id 'war'
	id 'org.springframework.boot' version '2.7.16'
	id 'io.spring.dependency-management' version '1.0.15.RELEASE'
}

group = 'com.kbhc'
version = '0.0.1-SNAPSHOT'

java {
	sourceCompatibility = '11'
}

configurations {
	compileOnly {
		extendsFrom annotationProcessor
	}
}

repositories {
	mavenCentral()
}

dependencies {
	implementation 'org.springframework.boot:spring-boot-starter-data-jpa'
	implementation 'org.springframework.boot:spring-boot-starter-validation'
	implementation 'org.springframework.boot:spring-boot-starter-web'
	implementation 'org.mybatis.spring.boot:mybatis-spring-boot-starter:2.3.1'
	compileOnly 'org.projectlombok:lombok'
	runtimeOnly 'com.mysql:mysql-connector-j'
	runtimeOnly 'org.postgresql:postgresql'
	annotationProcessor 'org.projectlombok:lombok'
	providedRuntime 'org.springframework.boot:spring-boot-starter-tomcat'
	testImplementation 'org.springframework.boot:spring-boot-starter-test'
	testImplementation 'org.mybatis.spring.boot:mybatis-spring-boot-starter-test:2.3.1'

	/*추가 의존성*/
	
	// JSP 관련 의존성 추가
	implementation 'org.apache.tomcat.embed:tomcat-embed-jasper'
	implementation 'javax.servlet:jstl'
	
	// JUnit5
	testImplementation("org.junit.platform:junit-platform-launcher:1.5.2") 
	testImplementation("org.junit.jupiter:junit-jupiter:5.5.2")

	// Qeury Log 설정
	implementation "org.bgee.log4jdbc-log4j2:log4jdbc-log4j2-jdbc4:1.16"
	
	// Json타입 설정
	implementation 'com.fasterxml.jackson.core:jackson-databind:2.13.0'
	
	// 화면 자동 반영
	implementation 'org.springframework.boot:spring-boot-devtools'
}

tasks.named('test') {
	useJUnitPlatform()
}
```

## 3. DB설정(application.yml)
```
spring:
  datasource:

    # Resd replica 구성
    master: # Write(Insert, Update, Delete)
      driver-class-name: net.sf.log4jdbc.sql.jdbcapi.DriverSpy
      jdbc-url: jdbc:log4jdbc:mysql://kms-mysql.mysql.database.azure.com:3306/test?serverTimezone=UTC
      read-only: false
      username: ******
      password: ******
      
    slave:  # Read(Select)
      driver-class-name: net.sf.log4jdbc.sql.jdbcapi.DriverSpy
      jdbc-url: jdbc:log4jdbc:mysql://kms-mysql-slave.mysql.database.azure.com:3306/test?serverTimezone=UTC
      read-only: true # Slave DB는 Read만
      username: ******
      password: ******
```
## 4. DataSource 설정
## [4-1] RoutingDataSource 생성
```
import org.springframework.jdbc.datasource.lookup.AbstractRoutingDataSource;
import org.springframework.transaction.support.TransactionSynchronizationManager;

public class RoutingDataSource extends AbstractRoutingDataSource {
    @Override
    protected Object determineCurrentLookupKey() { // (1)
        return (TransactionSynchronizationManager.isCurrentTransactionReadOnly()) ? "slave" : "master"; //(2)
    }
}
```

## [4-2] DataSourceConfig 설정
```
import java.util.HashMap;
import java.util.Map;

import javax.sql.DataSource;

import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.boot.jdbc.DataSourceBuilder;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.DependsOn;
import org.springframework.context.annotation.Primary;

@Configuration
public class DataSourceConfig {
	
	/* 1.  DataSource Bean 등록 */
	public static final String MASTER_DATASOURCE = "masterDataSource";
    public static final String SLAVE_DATASOURCE = "slaveDataSource";
    @Bean(name = MASTER_DATASOURCE)
    @ConfigurationProperties(prefix = "spring.datasource.master")
    public DataSource masterDataSource() {
        return DataSourceBuilder.create().build();
    }

    @Bean(name = SLAVE_DATASOURCE)
    @ConfigurationProperties(prefix = "spring.datasource.slave")
    public DataSource slaveDataSource() {
        return DataSourceBuilder.create().build();
    }
    
    /* 1.  DataSource Bean 등록 */
    @Bean
    @Primary
    @DependsOn({MASTER_DATASOURCE, SLAVE_DATASOURCE})
    public DataSource routingDataSource(
        @Qualifier(MASTER_DATASOURCE) DataSource masterDataSource,
        @Qualifier(SLAVE_DATASOURCE) DataSource slaveDataSource) {

        RoutingDataSource routingDataSource = new RoutingDataSource();

        Map<Object, Object> datasourceMap = new HashMap<>() {
            {
                put("master", masterDataSource);
                put("slave", slaveDataSource);
            }
        };

        routingDataSource.setTargetDataSources(datasourceMap);
        routingDataSource.setDefaultTargetDataSource(masterDataSource);

        return routingDataSource;
    }
}
```