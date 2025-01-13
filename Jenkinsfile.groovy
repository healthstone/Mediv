def UUID_DIR  = UUID.randomUUID().toString()
def url = "https://jenkins.snow1k.com/job/MedivhEmu/${BUILD_NUMBER}/"

pipeline {

    agent any

    stages {
        stage('Stop services') {
            steps {
                sh('set +e docker-compose -f docker/docker-compose.yml down')
            }
        }
        stage('Build Core') {
            steps {
                sh('docker build -f docker/core/Dockerfile --no-cache --progress=plain -t mediv-core .')
            }
        }
        stage('Build authserver') {
            steps {
                sh('docker build -f docker/authserver/Dockerfile --no-cache --progress=plain -t mediv-authserver .')
            }
        }
        stage('Build worldserver') {
            steps {
                sh('docker build -f docker/worldserver/Dockerfile --no-cache --progress=plain -t mediv-worldserver .')
            }
        }
        stage('Start services') {
            steps {
                sh('docker-compose -f docker/docker-compose.yml up -d')
            }
        }
    }

    post {
        always {
            script {
                String emoj = getEmoj(currentBuild.result)
                rocketSend channel: '#wowemu', message: "MedivhEmu - BUILD-STATUS : ${currentBuild.result}. Report: ${url}", emoji: emoj, rawMessage: true
            }
        }
    }

}

def String getEmoj(String status) {
    switch(status) {
        case 'SUCCESS': return ':smirk:'
        case 'FAILURE': return ':sob:'
        case 'UNSTABLE': return ':thinking:'
        default:
            return ':sob:'
    }
}